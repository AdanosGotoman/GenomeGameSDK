/*
Copyright(c) 2016-2021 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ==========================
#include "Spartan.h"
#include "World.h"
#include "Entity.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Components/Light.h"
#include "Components/Environment.h"
#include "Components/AudioListener.h"
#include "../Resource/ResourceCache.h"
#include "../Resource/ProgressTracker.h"
#include "../IO/FileStream.h"
#include "../Profiling/Profiler.h"
#include "../Rendering/Renderer.h"
#include "../Input/Input.h"
#include "../RHI/RHI_Device.h"
//=====================================

//= NAMESPACES ================
using namespace Genome::Math;
//=============================

namespace Genome
{
    World::World(Context* context) : ISubsystem(context)
    {
        // Subscribe to events
        SUBSCRIBE_TO_EVENT(EventType::WorldResolve, [this](Variant) { m_resolve = true; });
    }

    World::~World()
    {
        m_input     = nullptr;
        m_profiler  = nullptr;
    }

    bool World::Initialize()
    {
        m_input     = m_context->GetSubsystem<Input>();
        m_profiler  = m_context->GetSubsystem<Profiler>();

        CreateCamera();
        CreateEnvironment();
        CreateDirectionalLight();

        return true;
    }

    void World::Tick(float delta_time)
    {
        // If something is being loaded, don't tick as entities are probably being added
        if (IsLoading())
            return;

        SCOPED_TIME_BLOCK(m_profiler);

        // Tick entities
        {
            // Detect game toggling
            bool is_engine_mode_set = m_context->m_engine->EngineMode_IsSet(Engine_Game);

            const bool started    = is_engine_mode_set && m_was_in_editor_mode;
            const bool stopped    = !is_engine_mode_set && !m_was_in_editor_mode;
            m_was_in_editor_mode  = !is_engine_mode_set;

            // Start
            if (started)
            {
                for (std::shared_ptr<Entity>& entity : m_entities)
                {
                    entity->Start();
                }
            }

            // Stop
            if (stopped)
            {
                for (std::shared_ptr<Entity>& entity : m_entities)
                {
                    entity->Stop();
                }
            }

            // Tick
            for (std::shared_ptr<Entity>& entity : m_entities)
            {
                entity->Tick(delta_time);
            }
        }

        if (m_resolve)
        {
            // Update dirty entities
            {
                // Make a copy so we can iterate while removing entities
                auto entities_copy = m_entities;

                for (std::shared_ptr<Entity>& entity : entities_copy)
                {
                    if (entity->IsPendingDestruction())
                    {
                        _EntityRemove(entity);
                    }
                }
            }

            // Notify Renderer
            FIRE_EVENT_DATA(EventType::WorldResolved, m_entities);
            m_resolve = false;
        }
    }

    void World::New()
    {
        Clear();
    }

    bool World::SaveToFile(const std::string& filePathIn)
    {
        // Start progress report and timer
        auto progress_tracker = ProgressTracker::Get();
        progress_tracker.Reset(ProgressType::World);
        progress_tracker.SetIsLoading(ProgressType::World, true);
        progress_tracker.SetStatus(ProgressType::World, "Saving world...");
        const Stopwatch timer;
    
        // Add scene file extension to the filepath if it's missing
        auto file_path = filePathIn;
        if (FileSystem::GetExtensionFromFilePath(file_path) != EXTENSION_WORLD)
        {
            file_path += EXTENSION_WORLD;
        }
        m_name = FileSystem::GetFileNameNoExtensionFromFilePath(file_path);

        // Notify subsystems that need to save data
        FIRE_EVENT(EventType::WorldSave);

        // Create a prefab file
        auto file = std::make_unique<FileStream>(file_path, FileStream_Write);
        if (!file->IsOpen())
        {
            LOG_ERROR_GENERIC_FAILURE();
            return false;
        }

        // Only save root entities as they will also save their descendants
        auto root_actors = EntityGetRoots();
        const auto root_entity_count = static_cast<uint32_t>(root_actors.size());

        progress_tracker.SetJobCount(ProgressType::World, root_entity_count);

        // Save root entity count
        file->Write(root_entity_count);

        // Save root entity IDs
        for (const auto& root : root_actors)
        {
            file->Write(root->GetId());
        }

        // Save root entities
        for (const auto& root : root_actors)
        {
            root->Serialize(file.get());
            progress_tracker.IncrementJobsDone(ProgressType::World);
        }

        // Finish with progress report and timer
        progress_tracker.SetIsLoading(ProgressType::World, false);
        LOG_INFO("Saving took %.2f ms", timer.GetElapsedTimeMs());

        // Notify subsystems waiting for us to finish
        FIRE_EVENT(EventType::WorldSaved);

        return true;
    }

    bool World::LoadFromFile(const std::string& file_path)
    {
        if (!FileSystem::Exists(file_path))
        {
            LOG_ERROR("%s was not found.", file_path.c_str());
            return false;
        }

        // Open file
        auto file = std::make_unique<FileStream>(file_path, FileStream_Read);
        if (!file->IsOpen())
            return false;

        // Start progress report and timing
        auto progress_tracker = ProgressTracker::Get();
        progress_tracker.Reset(ProgressType::World);
        progress_tracker.SetIsLoading(ProgressType::World, true);
        progress_tracker.SetStatus(ProgressType::World, "Loading world...");
        const Stopwatch timer;

        // The Renderer will detect the world loading progress and stop (to avoid entity race conditions).
        // Because this loading function can be called by a different thread, we wait for it to stop first.
        Renderer* renderer = m_context->GetSubsystem<Renderer>();
        renderer->Stop();
        
        // Clear current entities
        Clear();

        renderer->Start();

        m_name = FileSystem::GetFileNameNoExtensionFromFilePath(file_path);

        // Notify subsystems that need to load data
        FIRE_EVENT(EventType::WorldLoad);

        // Load root entity count
        const uint32_t root_entity_count = file->ReadAs<uint32_t>();

        progress_tracker.SetJobCount(ProgressType::World, root_entity_count);

        // Load root entity IDs
        for (uint32_t i = 0; i < root_entity_count; i++)
        {
            std::shared_ptr<Entity> entity = EntityCreate();
            entity->SetId(file->ReadAs<uint32_t>());
        }

        // Serialize root entities
        for (uint32_t i = 0; i < root_entity_count; i++)
        {
            m_entities[i]->Deserialize(file.get(), nullptr);
            progress_tracker.IncrementJobsDone(ProgressType::World);
        }

        progress_tracker.SetIsLoading(ProgressType::World, false);
        LOG_INFO("Loading took %.2f ms", timer.GetElapsedTimeMs());

        FIRE_EVENT(EventType::WorldLoaded);

        return true;
    }

    bool World::IsLoading()
    {
        auto& progress_tracker = ProgressTracker::Get();

        const bool is_loading_model = progress_tracker.GetIsLoading(ProgressType::ModelImporter);
        const bool is_loading_scene = progress_tracker.GetIsLoading(ProgressType::World);

        return is_loading_model || is_loading_scene;
    }

    std::shared_ptr<Entity> World::EntityCreate(bool is_active /*= true*/)
    {
        std::shared_ptr<Entity> entity = m_entities.emplace_back(std::make_shared<Entity>(m_context));
        entity->SetActive(is_active);
        return entity;
    }

    bool World::EntityExists(const std::shared_ptr<Entity>& entity)
    {
        if (!entity)
            return false;

        return EntityGetById(entity->GetId()) != nullptr;
    }

    void World::EntityRemove(const std::shared_ptr<Entity>& entity)
    {
        if (!entity)
            return;

        // Mark for destruction but don't delete now
        // as the Renderer might still be using it.
        entity->MarkForDestruction();
        m_resolve = true;
    }

    std::vector<std::shared_ptr<Entity>> World::EntityGetRoots()
    {
        std::vector<std::shared_ptr<Entity>> root_entities;
        for (const auto& entity : m_entities)
        {
            if (entity->GetTransform()->IsRoot())
            {
                root_entities.emplace_back(entity);
            }
        }

        return root_entities;
    }

    const std::shared_ptr<Entity>& World::EntityGetByName(const std::string& name)
    {
        for (const auto& entity : m_entities)
        {
            if (entity->GetName() == name)
                return entity;
        }

        static std::shared_ptr<Entity> empty;
        return empty;
    }

    const std::shared_ptr<Entity>& World::EntityGetById(const uint32_t id)
    {
        for (const auto& entity : m_entities)
        {
            if (entity->GetId() == id)
                return entity;
        }

        static std::shared_ptr<Entity> empty;
        return empty;
    }

    void World::Clear()
    {
        // Notify any systems that the entities are about to be cleared
        FIRE_EVENT(EventType::WorldClear);
        m_context->GetSubsystem<Renderer>()->Clear();
        m_context->GetSubsystem<ResourceCache>()->Clear();

        // Clear the entities
        m_entities.clear();

        m_resolve = true;
    }

    // Removes an entity and all of it's children
    void World::_EntityRemove(const std::shared_ptr<Entity>& entity)
    {
        // Remove any descendants
        auto children = entity->GetTransform()->GetChildren();
        for (const auto& child : children)
        {
            EntityRemove(child->GetEntity()->GetPtrShared());
        }

        // Keep a reference to it's parent (in case it has one)
        auto parent = entity->GetTransform()->GetParent();

        // Remove this entity
        for (auto it = m_entities.begin(); it < m_entities.end();)
        {
            const auto temp = *it;
            if (temp->GetId() == entity->GetId())
            {
                it = m_entities.erase(it);
                break;
            }
            ++it;
        }

        // If there was a parent, update it
        if (parent)
        {
            parent->AcquireChildren();
        }
    }

    std::shared_ptr<Entity> World::CreateEnvironment()
    {
        std::shared_ptr<Entity> environment = EntityCreate();
        environment->SetName("Environment");
        environment->AddComponent<Environment>()->LoadDefault();

        return environment;
    }

    std::shared_ptr<Entity> World::CreateCamera()
    {
        ResourceCache* resource_cache   = m_context->GetSubsystem<ResourceCache>();
        const std::string dir_scripts        = resource_cache->GetResourceDirectory(ResourceDirectory::Scripts) + "/";

        std::shared_ptr<Entity> entity = EntityCreate();
        entity->SetName("Camera");
        entity->AddComponent<Camera>();
        entity->AddComponent<AudioListener>();
        entity->GetTransform()->SetPositionLocal(Vector3(0.0f, 1.0f, -5.0f));

        return entity;
    }

    std::shared_ptr<Entity> World::CreateDirectionalLight()
    {
        std::shared_ptr<Entity> light = EntityCreate();
        light->SetName("DirectionalLight");
        light->GetTransform()->SetRotationLocal(Quaternion::FromEulerAngles(30.0f, 30.0, 0.0f));
        light->GetTransform()->SetPosition(Vector3(0.0f, 10.0f, 0.0f));

        auto light_comp = light->AddComponent<Light>();
        light_comp->SetLightType(LightType::Directional);

        return light;
    }
}
