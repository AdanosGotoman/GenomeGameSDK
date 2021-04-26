//= INCLUDES ======================
#include "Spartan.h"
#include "../Core/FileSystem.h"
#include "../Rendering/Renderer.h"
#include "../Threading/Threading.h"
//=================================

//= NAMESPACES ================
using namespace std;
using namespace Genome::Math;
//=============================

namespace _Settings
{
    ofstream fout;
    ifstream fin;
    string file_name = "Spartan.ini";

    template <class T>
    void write_setting(ofstream& fout, const string& name, T value)
    {
        fout << name << "=" << value << endl;
    }

    template <class T>
    void read_setting(ifstream& fin, const string& name, T& value)
    {
        for (string line; getline(fin, line); )
        {
            const auto first_index = line.find_first_of('=');
            if (name == line.substr(0, first_index))
            {
                const auto lastindex = line.find_last_of('=');
                const auto read_value = line.substr(lastindex + 1, line.length());
                value = static_cast<T>(stof(read_value));
                return;
            }
        }
    }
}

namespace Genome
{
    Settings::Settings(Context* context) : ISubsystem(context)
    {
        m_context = context;

        // Register pugixml
        const auto major = to_string(PUGIXML_VERSION / 1000);
        const auto minor = to_string(PUGIXML_VERSION).erase(0, 1).erase(1, 1);
        RegisterThirdPartyLib("pugixml", major + "." + minor, "https://github.com/zeux/pugixml");

        // Register SPIRV-Cross
        RegisterThirdPartyLib("SPIRV-Cross", "2020-01-16", "https://github.com/KhronosGroup/SPIRV-Cross");

        // Register DirectXShaderCompiler
        RegisterThirdPartyLib("DirectXShaderCompiler", "1.6 - 1.5.0.2860", "https://github.com/microsoft/DirectXShaderCompiler");
    }

    Settings::~Settings()
    {
        Reflect();
        Save();
    }

    bool Settings::Initialize()
    {
        // Acquire default settings
        Reflect();

        if (FileSystem::Exists(_Settings::file_name))
        {
            Load();
            Map();
        }
        else
        {
            Save();
        }

        LOG_INFO("Resolution: %dx%d", static_cast<int>(m_resolution_render.x), static_cast<int>(m_resolution_render.y));
        LOG_INFO("FPS Limit: %f", m_fps_limit);
        LOG_INFO("Shadow resolution: %d", m_shadow_map_resolution);
        LOG_INFO("Anisotropy: %d", m_anisotropy);
        LOG_INFO("Max threads: %d", m_max_thread_count);

        return true;
    }

    void Settings::RegisterThirdPartyLib(const std::string& name, const std::string& version, const std::string& url)
    {
        m_third_party_libs.emplace_back(name, version, url);
    }

    void Settings::Save() const
    {
        // Create a settings file
        _Settings::fout.open(_Settings::file_name, ofstream::out);

        // Write the settings
        _Settings::write_setting(_Settings::fout, "bFullScreen", m_is_fullscreen);
        _Settings::write_setting(_Settings::fout, "bIsMouseVisible", m_is_mouse_visible);
        _Settings::write_setting(_Settings::fout, "iResolutionOutputWidth", m_resolution_output.x);
        _Settings::write_setting(_Settings::fout, "iResolutionOutputHeight", m_resolution_output.y);
        _Settings::write_setting(_Settings::fout, "iResolutionRenderWidth", m_resolution_render.x);
        _Settings::write_setting(_Settings::fout, "iResolutionRenderHeight", m_resolution_render.y);
        _Settings::write_setting(_Settings::fout, "iShadowMapResolution", m_shadow_map_resolution);
        _Settings::write_setting(_Settings::fout, "iAnisotropy", m_anisotropy);
        _Settings::write_setting(_Settings::fout, "iTonemapping", m_tonemapping);
        _Settings::write_setting(_Settings::fout, "fFPSLimit", m_fps_limit);
        _Settings::write_setting(_Settings::fout, "iMaxThreadCount", m_max_thread_count);
        _Settings::write_setting(_Settings::fout, "iRendererFlags", m_renderer_flags);

        // Close the file.
        _Settings::fout.close();
    }

    void Settings::Load()
    {
        // Create a settings file
        _Settings::fin.open(_Settings::file_name, ifstream::in);

        // Read the settings
        _Settings::read_setting(_Settings::fin, "bFullScreen", m_is_fullscreen);
        _Settings::read_setting(_Settings::fin, "bIsMouseVisible", m_is_mouse_visible);
        _Settings::read_setting(_Settings::fin, "iResolutionOutputWidth", m_resolution_output.x);
        _Settings::read_setting(_Settings::fin, "iResolutionOutputHeight", m_resolution_output.y);
        _Settings::read_setting(_Settings::fin, "iResolutionRenderWidth", m_resolution_render.x);
        _Settings::read_setting(_Settings::fin, "iResolutionRenderHeight", m_resolution_render.y);
        _Settings::read_setting(_Settings::fin, "iShadowMapResolution", m_shadow_map_resolution);
        _Settings::read_setting(_Settings::fin, "iAnisotropy", m_anisotropy);
        _Settings::read_setting(_Settings::fin, "iTonemapping", m_tonemapping);
        _Settings::read_setting(_Settings::fin, "fFPSLimit", m_fps_limit);
        _Settings::read_setting(_Settings::fin, "iMaxThreadCount", m_max_thread_count);
        _Settings::read_setting(_Settings::fin, "iRendererFlags", m_renderer_flags);

        // Close the file.
        _Settings::fin.close();

        m_loaded = true;
    }

    void Settings::Reflect()
    {
        Renderer* renderer = m_context->GetSubsystem<Renderer>();

        m_fps_limit = m_context->GetSubsystem<Timer>()->GetTargetFps();
        m_max_thread_count = m_context->GetSubsystem<Threading>()->GetThreadCountSupport();
        m_is_fullscreen = renderer->GetIsFullscreen();
        m_resolution_output = renderer->GetResolutionOutput();
        m_resolution_render = renderer->GetResolutionRender();
        m_shadow_map_resolution = renderer->GetOptionValue<uint32_t>(Renderer_Option_Value::ShadowResolution);
        m_anisotropy = renderer->GetOptionValue<uint32_t>(Renderer_Option_Value::Anisotropy);
        m_tonemapping = renderer->GetOptionValue<uint32_t>(Renderer_Option_Value::Tonemapping);
        m_renderer_flags = renderer->GetOptions();
    }

    void Settings::Map() const
    {
        Renderer* renderer = m_context->GetSubsystem<Renderer>();

        m_context->GetSubsystem<Timer>()->SetTargetFps(m_fps_limit);
        renderer->SetIsFullscreen(m_is_fullscreen);
        renderer->SetResolutionOutput(static_cast<uint32_t>(m_resolution_output.x), static_cast<uint32_t>(m_resolution_output.y));
        renderer->SetResolutionRender(static_cast<uint32_t>(m_resolution_render.x), static_cast<uint32_t>(m_resolution_render.y));
        renderer->SetOptionValue(Renderer_Option_Value::ShadowResolution, static_cast<float>(m_shadow_map_resolution));
        renderer->SetOptionValue(Renderer_Option_Value::Anisotropy, static_cast<float>(m_anisotropy));
        renderer->SetOptionValue(Renderer_Option_Value::Tonemapping, static_cast<float>(m_tonemapping));
        renderer->SetOptions(m_renderer_flags);
    }
}
