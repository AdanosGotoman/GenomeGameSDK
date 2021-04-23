#pragma once
#include <string>

namespace Genome
{
    class ClassDef
    {
    public:
        std::string className;
        std::string baseClassName;
        std::string scriptClassName;

        ClassDef* classDef;
    };

    class GameObject
    {
        friend ClassDef;

    public:
        int refCtr;
        unsigned short hashIndex;
        GameObject* hashNext;
        std::string objName;

        void Init();
        GameObject();
        int Release();
        GameObject* CreateCopy();
        std::string const& GetObjName() const;
        int SetObjName(std::string const&);
    };
}
