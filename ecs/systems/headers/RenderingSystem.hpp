#pragma once

#include "Components.hpp"
#include "ISystems.hpp"
#include "IRenderer.hpp"
#include "SparseArray.hpp"
#include "ZipperIterator.hpp"
#include "Registry.hpp"

namespace Systems 
{
    class RenderingSystem : public ISystem
    {

        public:
            RenderingSystem(ECS::SparseArray<Animatable> &animatables,
                            ECS::SparseArray<Transformable> &transformables,
                            ECS::SparseArray<Renderable> &renderables,
                            ECS::SparseArray<TextRenderable> &textRenderables,
                            ECS::SparseArray<ParallaxScroller> &parallaxScrollers,
                            ECS::SparseArray<Shape> &shapes,
                            ECS::SparseArray<Color> &colors,
                            ECS::SparseArray<Audio> &sounds,
                            std::shared_ptr<IRenderer> renderer)
                            : _animatables(animatables),
                              _transformables(transformables), _renderables(renderables), 
                              _textRenderables(textRenderables), _parallaxScrollers(parallaxScrollers),
                               _colors(colors), _shapes(shapes), _sounds(sounds),
                              _renderer(renderer) {}
            
            void update(float dt) override
            {
                _renderer->clear();

                for (auto &&[sou] : Zipper(_sounds)) {
                    if (sou.has_value()) {
                        if (!sou->isMusic){
                            _renderer->playSounds(sou->audioPath, sou->volume, sou->pitch, sou->isPlaying, sou->loop);
                        }
                        else {
                            _renderer->playMusic(sou->audioPath, sou->volume, sou->isPlaying, sou->loop);
                        }
                    }
                }

                for (auto &&[parallax] : Zipper(_parallaxScrollers)) {
                    if (parallax.has_value()) {
                        _renderer->drawParallaxLayers(
                            parallax->textureId,
                            parallax->offsetX,
                            parallax->offsetY,
                            parallax->scrollSpeedX,
                            parallax->scrollSpeedY,
                            parallax->zLayer,
                            parallax->rotation,
                            parallax->scale,
                            dt,
                            parallax->isVisible,
                            parallax->isAnimated,
                            parallax->currentFrame
                        );
                    }
                }

                for (auto &&[trans, rend, color] : Zipper(_transformables, _renderables, _colors)) {
                    if (trans.has_value() && rend.has_value()) {
                        _renderer->drawTexture(
                            rend->textureId,
                            trans->positionX,
                            trans->positionY,
                            trans->rotation,
                            trans->scaleX,
                            trans->scaleY,
                            rend->zOrder,
                            rend->currFrame,
                            rend->visible,
                            rend->frameWidth,
                            rend->frameHeight,
                            rend->animated,
                            rend->originX,
                            rend->originY,
                            color->r,
                            color->g,
                            color->b,
                            color->a
                        );
                    }
                }

                for (auto &&[txt] : Zipper( _textRenderables)) {
                    if (txt.has_value()) {
                        _renderer->drawText(
                            txt->text,
                            txt->fontPath,
                            txt->fontSize,
                            txt->x,
                            txt->y,
                            txt->zOrder,
                            txt->visible,
                            txt->a,
                            txt->r,
                            txt->g,
                            txt->b
                        );
                    }
                }
                for (auto &&[sha, col, tra] : Zipper(_shapes, _colors, _transformables)) {
                    if (sha.has_value() && col.has_value() && tra.has_value()) {
                        _renderer->drawShape(
                            sha->shapeType, tra->positionX, tra->positionY, tra->rotation, sha->isVisible,
                             col->r, col->g, col->b, col->a, sha->sizeX, sha->sizeY, sha->radius
                        );
                    }
                }

            }

        private:
            ECS::SparseArray<Animatable> &_animatables;
            ECS::SparseArray<Transformable> &_transformables;
            ECS::SparseArray<Renderable> &_renderables;
            ECS::SparseArray<TextRenderable> &_textRenderables;
            ECS::SparseArray<ParallaxScroller> &_parallaxScrollers;
            ECS::SparseArray<Color> &_colors;
            ECS::SparseArray<Shape> &_shapes;
            ECS::SparseArray<Audio> &_sounds;
            std::shared_ptr<IRenderer> _renderer;
    };
}
