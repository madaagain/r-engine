#pragma once

#include <iostream>
#include "Components.hpp"
#include "ISystems.hpp"
#include "Registry.hpp"

namespace Systems 
{
    constexpr float FADE_IN_TIME  = 4.0f;
    constexpr float FADE_OUT_TIME = 4.0f;

    class FadeTransitionSystem : public ISystem 
    {
    public:
        FadeTransitionSystem(ECS::Registry &registry, float &dt)
            : _registry(registry), _dt(dt),
              _sceneTime(0.0f),
              _fadingIn(false), _fadingOut(false),
              _currentLevel(1)
        {
        }

        void update(float dt) override
        {
            if (!isSceneResetting() && (_fadingIn && !_fadingOut))
                return;
            auto &shapes = _registry.getComponents<Shape>();
            auto &colors = _registry.getComponents<Color>();
            auto &scenes = _registry.getComponents<Scene>();
            
            
            for (auto &&[shape, color] : Zipper(shapes, colors)) {
                if (!shape.has_value() || !color.has_value())
                    continue;
                    
                    
                for (auto &&[scene] : Zipper(scenes)) {
                    if (!scene.has_value()) 
                        continue;
                        
                        
                    if (!_fadingIn && !_fadingOut) {
                        _fadingIn = true;
                        _sceneTime = 0.0f;
                        color->a = 0.0f;
                    }
                    
                    
                    if (_fadingIn) {
                        _sceneTime += dt;
                        color->a = 255.0f * (_sceneTime / FADE_IN_TIME);
                        if (color->a > 255.0f) {
                            color->a = 255.0f;
                        }
                        
                        
                        if (_sceneTime >= FADE_IN_TIME) {
                            _fadingIn = false;
                            _sceneTime = 0.0f;
                            scene->nextParallax = (_currentLevel == 1) ? "LevelTwo" : "LevelThree";
                            scene->nextParallax = (_currentLevel == 1) ? "LevelTwo" : "LevelThree";
                            _fadingOut = true;

                            if (_currentLevel == 1) {
                                changeScene("LevelTwo");
                                _currentLevel = 2;
                            } else if (_currentLevel == 2) {
                                changeScene("LevelThree");
                                _currentLevel = 3;
                            }
                            else if (_currentLevel == 3) {
                                changeScene("CreditScene");
                                _currentLevel = 4;
                            }
                        }
                    }
                    else if (_fadingOut) {
                        _sceneTime += dt;
                        color->a = 255.0f - (255.0f * (_sceneTime / FADE_OUT_TIME));
                        if (color->a < 0.0f) {
                            color->a = 0.0f;
                        }
                        
                        
                        if (_sceneTime >= FADE_OUT_TIME) {
                            color->a = 0.0f;
                            _fadingOut = false;
                            _sceneTime = 0.0f;
                            scene->nextParallax.clear();

                            if (_currentLevel == 1) {
                                _fadingIn = true;
                            }
                        }
                    }
                }
            }
        }

    protected:

    private:
        ECS::Registry &_registry;
        float &_dt;
        float _sceneTime;
        bool _fadingIn;
        bool _fadingOut;
        int _currentLevel;

        bool isSceneResetting()
        {
            auto &sceneComp = _registry.getComponents<Scene>();
            for (auto &&[sce] : Zipper(sceneComp)) {
                if (sce.has_value() && sce->currentScene == "SceneTransition") {
                    stopWinMusic();
                    return true;
                }
            }
            return false;
        }

        void changeScene(const std::string &newScene)
        {
            auto &sceneComp = _registry.getComponents<Scene>();
            for (auto &&[sce] : Zipper(sceneComp)) {
                if (sce.has_value()) {
                    sce->currentScene = newScene;
                }
            }
        }

        void revivePlayers()
        {
            auto &scenes = _registry.getComponents<Scene>();
            auto &player = _registry.getComponents<Player>();
            auto &hps = _registry.getComponents<Health>();
            auto &renderables = _registry.getComponents<Renderable>();
            bool revive = false;

            for (auto &&[sce] : Zipper(scenes)) {
                if (!sce.has_value() || sce->currentScene == "SceneTransition") {
                    revive = true;
                    
                }
            }
            for (auto &&[idx, hp, pl, rend] : indexedZipper(hps, player, renderables)) {
                if (hp.has_value() && pl.has_value() && rend.has_value()) {
                    if (hp->currentHealth <= 0 && revive) {
                        hp->currentHealth = hp->maxHealth;
                    }
                    if (hp->currentHealth == hp->maxHealth && !revive) {
                        rend->visible = true;
                    }
                }
            }
            revive = false;
        }

        void stopWinMusic()
        {
            auto &audios = _registry.getComponents<Audio>();
            for (auto &&[audio] : Zipper(audios)) {
                if (audio.has_value()) {
                    if (audio->audioPath == "../../client/assets/musics/win-level.ogg"){
                        audio->isPlaying = false;
                    }
                }
            }
        }

    };
}