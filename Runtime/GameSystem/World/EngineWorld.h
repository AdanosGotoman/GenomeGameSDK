#pragma once
#include "../GameSystem/SharedBase/GameObject.h"
#include "../../World/Components/Camera.h"

namespace Genome
{
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

        const bool writeBin = false;

        void SetProgressBar();
        void GetProgressBar();
        void Render(Camera& cam);
    };
}
