#pragma once

#include "ISystems.hpp"
#include <iostream>

namespace Systems
{
    class AnimationSystem : public ISystem
    {
        public:
            AnimationSystem(ECS::SparseArray<Animatable> &animatables,
                            ECS::SparseArray<ClockComponent> &dtComponents,
                            ECS::SparseArray<Renderable> &renderables,
                            ECS::SparseArray<ParallaxScroller> &parallaxes)
                : _animatables(animatables), _dtComponents(dtComponents), _renderables(renderables), _parallaxes(parallaxes) {}

            void update(float) override
            {
                for (auto &&[anim, clock, rend] : Zipper(_animatables, _dtComponents, _renderables)) {
                    // if (rend->animated == 0) {
                    //     continue;
                    // }
                    if (anim.has_value() && clock.has_value() && rend.has_value()) {
                        auto it = anim->animationMap.find(anim->animState);
                        if (it != anim->animationMap.end()) {
                            anim->currAnimation = it->second;
                        }
                        Animation &currentAnim = anim->animations[anim->currAnimation];
                        anim->animationTime += clock->dt;
                        /*
                        get animation state
                        */                        
                    //    std::cout << "anim time: " << anim->animationTime << std::endl;
                        if (anim->animationTime >= currentAnim.frameDuration) {
                            anim->animationTime = 0.0f;
                            if (currentAnim.isBackward) {
                                anim->currentFrame--;
                                if (anim->currentFrame < currentAnim.startFrame) {
                                    anim->currentFrame = currentAnim.startFrame + 1;
                                    currentAnim.isBackward = false;
                                }
                            } else {
                                anim->currentFrame++;
                                if (anim->currentFrame > currentAnim.endFrame) {
                                    if (currentAnim.isLooping) {
                                        anim->currentFrame = currentAnim.startFrame;
                                    } else {
                                        anim->currentFrame = currentAnim.endFrame - 1;
                                        currentAnim.isBackward = true;
                                    }
                                }
                            }
                            rend->currFrame = anim->currentFrame;
                        }

                        int frameIndex = rend->currFrame;
                        int frameWidth = rend->width / currentAnim.frameCount;
                        int frameHeight = rend->height;
                        int framesPerRow = rend->width / frameWidth;
                        int frameX = (frameIndex % framesPerRow) * frameWidth;
                        int frameY = currentAnim.row * frameHeight;
                        rend->originX = static_cast<float>(frameX);
                        rend->originY = static_cast<float>(frameY);
                        rend->animated = true;
                        rend->frameWidth = frameWidth;
                        rend->frameHeight = frameHeight;
                    }
                }
                for (auto &&[anim, clock, para] : Zipper(_animatables, _dtComponents, _parallaxes)) {
                    if (anim.has_value() && clock.has_value() && para.has_value()) {
                        auto it = anim->animationMap.find(anim->animState);
                        if (it != anim->animationMap.end()) {
                            anim->currAnimation = it->second;
                        }
                        Animation &currentAnim = anim->animations[anim->currAnimation];
                        anim->animationTime += clock->dt;

                        if (anim->animationTime >= currentAnim.frameDuration) {
                            anim->animationTime = 0.0f;
                            if (currentAnim.isBackward) {
                                anim->currentFrame--;
                                if (anim->currentFrame < currentAnim.startFrame) {
                                    anim->currentFrame = currentAnim.startFrame + 1;
                                    currentAnim.isBackward = false;
                                }
                            } else {
                                anim->currentFrame++;
                                if (anim->currentFrame > currentAnim.endFrame) {
                                    if (currentAnim.isLooping) {
                                        anim->currentFrame = currentAnim.startFrame;
                                    } else {
                                        anim->currentFrame = currentAnim.endFrame - 1;
                                        currentAnim.isBackward = true;
                                    }
                                }
                            }
                            para->currentFrame = anim->currentFrame;
                        }

                        para->isAnimated = true;
                    }
                }
            }

        private:
            ECS::SparseArray<Animatable>& _animatables;
            ECS::SparseArray<ClockComponent>& _dtComponents;
            ECS::SparseArray<Renderable>& _renderables;
            ECS::SparseArray<ParallaxScroller>& _parallaxes;
    };

}