#pragma once
#include "../GameSystem/SharedBase/GameObject.h"
#include "../../World/Components/Camera.h"

namespace Genome
{
    class Entity;

    class EngineWorld : public GameObject
    {
    public:
        enum class LoadMode
        {
            gWRL_LOAD_GAME,
        };
        enum class SaveMode
        {
            gWRL_SAVE_GAME,
        };

        virtual bool LoadWorld(std::string& fileName, const LoadMode loadMode);
        virtual bool SaveWorld(std::string& fileName, const SaveMode saveMode);
        virtual bool CreateWorld(void);
        virtual bool DestroyWorld(void);

        std::shared_ptr<Entity> CreateEntity(bool isActive = true);
        void RemoveEntity(const std::shared_ptr<Entity>& entity);

        const bool writeBin = false;

        void SetProgressBar();
        void GetProgressBar();
        void Render(Camera& cam);
    };
}
