#pragma once

#include <string>

class IRenderer
{
    public:
        virtual ~IRenderer() = default;

        virtual void clear() = 0;
        
        virtual void present() = 0;

        virtual void drawTexture(
            const std::string &textureId, float x, float y,
            float rotation = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f,
            int zOrder = 0, int frame = 0, bool isVisible = false,
            int width = 0, int height = 0, bool animated = false,
            float rectOriginX = 0.0f, float rectOriginY = 0.0f, float r = 255.0f, 
            float g= 255.0f, float b = 255.0f, float a= 255.0f
        ) = 0;

        virtual void addParallaxLayer(const std::string &textureId, float speedX,
                                      float speedY, int zLayer) = 0;

        virtual void drawParallaxLayers(
            const std::string &textureId, float &offsetX, float &offsetY,
            float speedX, float speedY, int zLayer, float &rotation, float scale, float dt, bool isVisible, bool isAnimated, int currentFrame
        ) = 0;

        virtual void drawShape(
            const std::string &shapeType, float x, float y,
            float rotation, bool isVisible, float r, float g, float b, float a,
            float sizeX, float sizeY, float radius
        ) = 0;

        virtual void playSounds(
            const std::string &soundFile, float volume,
            float pitch, bool isPlaying, bool isLoop
        ) = 0;

        virtual void drawText(
            const std::string &text, const std::string &fontPath, int fontSize,
            int x, int y, int zOrder, bool visible, float a, float r, float g, float b
        ) = 0;

        virtual void cleanupSounds() = 0;
        
        virtual void cleanupMusic() = 0;

        virtual void toggleBoundingBoxes(bool enable) = 0;

        virtual void playMusic(const std::string &musicFile, float volume, bool isPlaying, bool isLoop) = 0;
};

