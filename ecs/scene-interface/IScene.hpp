#pragma once

class IScene 
{
    public:
        virtual ~IScene() = default;
        virtual void initRegistry() = 0;
        virtual void initSystems() = 0;
        virtual void killEntities() = 0;
};
