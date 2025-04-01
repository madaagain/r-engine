

#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <unordered_map>

struct Color
{
    float r;
    float g;
    float b;
    float a;
    Color(float r = 0.0f, float g= 0.0f, float b= 0.0f, float a= 0.0f)
        : r(r), g(g), b(b), a(a) {}

    auto as_tuple() const {
        return std::make_tuple(r,g,b,a);
    }
};

struct Shape //this can't be anyshape obviously i'm braindead;..
{
    std::string shapeType;
    float sizeX;
    float sizeY;
    float radius;
    bool isVisible;

    Shape(const std::string &shapeType = "", float sizeX = 0.0f, float sizeY = 0.0f, float radius = 0.0f, bool isVisible = false)
        : shapeType(shapeType), sizeX(sizeX), sizeY(sizeY), radius(radius), isVisible(isVisible){}

    auto as_tuple() const {
        return std::make_tuple(shapeType, sizeX, sizeY, radius, isVisible);
    }
};

struct Transformable
{
    float positionX;
    float positionY;
    float rotation;
    float scaleX;
    float scaleY;
    Transformable(float x = 0.0f, float y = 0.0f, float rot = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f)
        : positionX(x), positionY(y), rotation(rot), scaleX(scaleX), scaleY(scaleY) {}

    auto as_tuple() const {
        return std::make_tuple(positionX, positionY, rotation, scaleX, scaleY);
    }
};

struct Velocity
{
    float vx, vy;
    Velocity(float vx = 0.0f, float vy = 0.0f) : vx(vx), vy(vy){}

    auto as_tuple() const {
        return std::make_tuple(vx, vy);
    }
};

struct Controllable
{
    bool moveLeft;
    bool moveRight;
    bool moveUp;
    bool moveDown;
    bool attack;
    uint32_t clientId;
    bool lastAttack;
    bool fKey;

    Controllable()
        : moveLeft(false), moveRight(false), moveUp(false), moveDown(false), attack(false), clientId(0), lastAttack(false), fKey(false) {}

    Controllable(bool moveLeft, bool moveRight, bool moveUp, bool moveDown, bool attack, bool fKey)
        : moveLeft(moveLeft), moveRight(moveRight), moveUp(moveUp), moveDown(moveDown), attack(attack), clientId(0), lastAttack(false), fKey(fKey) {}

    Controllable(bool moveLeft, bool moveRight, bool moveUp, bool moveDown, bool attack, uint32_t clientId, bool fKey)
        : moveLeft(moveLeft), moveRight(moveRight), moveUp(moveUp), moveDown(moveDown), attack(attack), clientId(clientId), lastAttack(false), fKey(fKey) {}

    auto as_tuple() const {
        return std::make_tuple(moveLeft, moveRight, moveUp, moveDown, attack, clientId, fKey);
    }
};

struct Missile
{
    float damage;
    uint32_t ownerId;

    Missile(float dmg = 10.0f, uint32_t owner = 0)
        : damage(dmg), ownerId(owner) {}

    auto as_tuple() const {
        return std::make_tuple(damage, ownerId);
    }
};

struct Force
{
    bool isAttached;
    bool isShooting;
    bool isAttachedFront;

    Force(bool isAttached = false, bool isShooting  = false, bool isAttachedFront = false) : isAttached(isAttached), isShooting(isShooting), isAttachedFront(isAttachedFront) {}

    auto as_tuple() const {
        return std::make_tuple(isAttached, isShooting, isAttachedFront);
    }
};

struct Score
{
    int totalScore;
    int score;
    bool increase;

    Score(int totalScore = 0, int score = 0, bool increase = false) : totalScore(totalScore), score(score), increase(increase) {}

    auto as_tuple() const {
        return std::make_tuple(totalScore, score, increase);
    }
};

struct Player
{
    uint32_t playerId;

    Player() : playerId(0) {}
    Player(uint32_t id) : playerId(id) {}

    auto as_tuple() const {
        return std::make_tuple(playerId);
    }
};

struct Health
{
    int currentHealth;
    int maxHealth;
    bool isInvicible;
    int invicibilitypts;
    Health(int currentHealth = 0, int maxHealth = 0, bool isInvicible = false, int invicibilitypts = 0)
        : currentHealth(currentHealth), maxHealth(maxHealth), isInvicible(isInvicible), invicibilitypts(invicibilitypts) {}

    auto as_tuple() const {
        return std::make_tuple(currentHealth, maxHealth, isInvicible, invicibilitypts);
    }
};

struct Renderable
{
    std::string textureId;
    int width;
    int height;
    float originX;
    float originY;
    int zOrder;
    bool visible;
    bool animated;
    int frameWidth;
    int frameHeight;
    int currFrame;

    Renderable(const std::string &textureId = "???",
               int width = 0, int height = 0, float originX = 0.0f,
               float originY = 0.0f, int zOrder = 0, bool visible = false,
               bool animated = false, int frameWidth = 0, int frameHeight = 0, int currFrame = 0)
                : textureId(textureId), width(width), height(height), originX(originX), originY(originY),
                  zOrder(zOrder), visible(visible), animated(animated), frameWidth(frameWidth),
                  frameHeight(frameHeight), currFrame(currFrame) {
                    frameWidth = width;
                    frameHeight = height;
                }

    auto as_tuple() const {
        return std::make_tuple(textureId, width, height, originX, originY, zOrder, visible, animated, frameWidth, frameHeight, currFrame);
    }
};

struct TextRenderable
{
    std::string GUItype;
    std::string text;
    std::string fontPath;
    int fontSize;
    int x;
    int y;
    int zOrder;
    bool visible;
    float a;
    float r;
    float g;
    float b;

    TextRenderable(const std::string &GUItype = "",const std::string &text = "", const std::string &fontPath = "", int fontSize = 0, int x = 0, int y = 0, int zOrder = 0, bool visible = false, float a = 0.0, float r = 0.0, float g = 0.0, float b = 0.0)
        : GUItype(GUItype), text(text), fontPath(fontPath), fontSize(fontSize), x(x), y(y), zOrder(zOrder), visible(visible), a(a), r(r), g(g), b(b) {}

    auto as_tuple() const {
        return std::make_tuple(GUItype, text, fontPath, fontSize, x, y, zOrder, visible, a, r, g, b);
    }
};

struct Scene
{
    std::string currentScene;
    bool isPaused;
    bool isResetting;

    std::string nextParallax;

    Scene(const std::string &scene = "LevelOne",
          bool paused = false,
          bool isResetting = false,
          const std::string &nextP = "")
        : currentScene(scene),
          isPaused(paused),
          isResetting(isResetting),
          nextParallax(nextP) {}
    auto as_tuple() const {
        return std::make_tuple(currentScene, isPaused, isResetting, nextParallax);
    }
};


struct AIState
{
    bool isPatrolling;
    bool isDead;
    bool isAttacking;

    AIState(bool isPatrolling, bool isDead, bool isAttacking) : isPatrolling(isPatrolling), isDead(isDead), isAttacking(isAttacking){}

    auto as_tuple() const {
        return std::make_tuple(isPatrolling, isDead, isAttacking);
    }
};

struct BossAIState
{
    bool isDead;
    bool isAttacking;
    bool switchState;

    BossAIState(bool isDead, bool isAttacking, bool switchState) : isDead(isDead), isAttacking(isAttacking), switchState(switchState){}

    auto as_tuple() const {
        return std::make_tuple(isDead, isAttacking, switchState);
    }
};

struct BydosAIComponent
{
    int damage;
    float time;
    float frequency;
    float amplitude;
    BydosAIComponent(int damage, float time, float frequency, float amplitude)
        : damage(damage), time(time), frequency(frequency), amplitude(amplitude) {}
};

struct RobotAIComponent
{
    int damage;
    float time;
    float frequency;
    float amplitude;
    RobotAIComponent(int damage, float time, float frequency, float amplitude)
        : damage(damage), time(time), frequency(frequency), amplitude(amplitude) {}
};

struct BossAIComponent
{
    int damage;
    float time;
    float frequency;
    float amplitude;
    BossAIComponent(int damage, float time, float frequency, float amplitude)
        : damage(damage), time(time), frequency(frequency), amplitude(amplitude) {}

    auto as_tuple() const {
        return std::make_tuple(damage, time, frequency, amplitude);
    }
};

struct ProtectiveOrbState
{
    bool isAttacking;
    bool isDetached;
    bool isReturning;
    float originalAngle;
    float currentDistance;
    float targetX;
    float targetY;

    ProtectiveOrbState(float angle = 0.0f, float distance = 0.0f, bool isAttacking = false, bool isDetached = false, bool isReturning = false, float targetX = 0.0f, float targetY = 0.0f)
        : isAttacking(isAttacking), isDetached(isDetached), isReturning(isReturning), originalAngle(angle), currentDistance(distance), targetX(targetX), targetY(targetY)
    {}

    auto as_tuple() const {
        return std::make_tuple(isAttacking, isDetached, isReturning, originalAngle, currentDistance, targetX, targetY);
    }
};

struct Audio
{
    std::string audioPath;
    bool isPlaying;
    bool loop;
    float volume;
    float pitch;
    bool isMusic;
    // bool delay;

    Audio(const std::string &audioPath = "",
          bool isPlaying = false, bool loop = false,
          float volume = 0.0f, float pitch = 0.0f, bool isMusic = false)
           : audioPath(audioPath), isPlaying(isPlaying), loop(loop),
           volume(volume), pitch(pitch), isMusic(isMusic){}

    auto as_tuple() const {
        return std::make_tuple(audioPath, isPlaying, loop, volume, pitch, isMusic);
    }
};

struct Animation
{
    int startFrame;
    int endFrame;
    bool isLooping;
    bool isBackward;
    int frameCount;
    int row;
    float frameDuration;

    auto as_tuple() const {
        return std::make_tuple(startFrame, endFrame, isLooping, isBackward, frameCount, row, frameDuration);
    }
};

enum class CollisionCategory : uint8_t
{
    None = 0,
    Player = 1 << 0,
    Enemy = 1 << 1,
    PlayerMissile = 1 << 2,
    EnemyMissile = 1 << 3,
    Environment = 1 << 4,
    Heal = 1 << 5,
    ShieldBonus = 1 << 6
};
inline bool canCollide(uint8_t bitMask, CollisionCategory category) {
    return ((bitMask & static_cast<uint8_t>(category)) != 0);
}

struct Collidable
{
    float width;
    float height;
    bool isColliding;
    CollisionCategory category;
    uint8_t bitMask;

    Collidable(float w = 0.0f, float h = 0.0f, CollisionCategory cat = CollisionCategory::None, uint8_t m = 0)
        : width(w), height(h), isColliding(false), category(cat), bitMask(m) {}

    Collidable(float w, float h, bool colliding, CollisionCategory cat, uint8_t m)
        : width(w), height(h), isColliding(colliding), category(cat), bitMask(m) {}
};

struct MultiCollidable
{
    std::vector<Collidable> collidables;
};

struct Animatable
{
    std::vector<Animation> animations;
    std::unordered_map<std::string, int> animationMap;
    int currAnimation;
    float animationTime;
    int currentFrame;
    std::string animState;

    Animatable(std::vector<Animation> anims = {}, int currFrame = 0, int currAnim = 0, float animTime = 0.0f, std::unordered_map<std::string, int> animationMap = {}, std::string animState = "")
        : animations(std::move(anims)), animationMap(animationMap), currAnimation(currAnim), animationTime(animTime), currentFrame(currFrame), animState(animState) {}

    auto as_tuple() const {
        return std::make_tuple(animations, animationMap, currAnimation, animationTime, currentFrame, animState);
    }
};

struct ShieldedBossComponent
{
    int requiredOrbs;
    int activeOrbs;
    bool isVulnerable;
    float orbAttackTimer;

    ShieldedBossComponent(int required = 20)
        : requiredOrbs(required), activeOrbs(required), isVulnerable(false), orbAttackTimer(0.0f) {}

    auto as_tuple() const {
        return std::make_tuple(requiredOrbs, activeOrbs, isVulnerable, orbAttackTimer);
    }
};

struct ParallaxScroller
{
    std::string textureId;
    float scrollSpeedX;
    float scrollSpeedY;
    int zLayer;
    float offsetX;
    float offsetY;
    float rotation;
    float scale;
    bool isVisible;
    bool isAnimated;
    int currentFrame;

    ParallaxScroller(const std::string &textureId = "", float speedX = 0.0f, float speedY = 0.0f, int zLayer = 0, float offsetX = 0.0f,
                    float offsetY = 0.0f, float rotation = 0, float scale = 0, bool isVisible = true, bool isAnimated = false, int currentFrame = 0)
                    : textureId(textureId), scrollSpeedX(speedX), scrollSpeedY(speedY), zLayer(zLayer), offsetX(offsetX),
                      offsetY(offsetY), rotation(rotation), scale(scale), isVisible(isVisible), isAnimated(isAnimated), currentFrame(currentFrame) {}

    auto as_tuple() const {
        return std::make_tuple(textureId, scrollSpeedX, scrollSpeedY, zLayer, offsetX, offsetY, rotation, scale, isVisible, isAnimated, currentFrame);
    }
};

struct ClockComponent
{
    float dt;

    auto as_tuple() const {
        return std::make_tuple(dt);
    }
};

struct Shield
{
    int damage;
    float frequency;
    float amplitude;
    float time;

    Shield(int damage = 0, float frequency = 0.0f, float amplitude = 0.0f, float time = 0.0f)
        : damage(damage), frequency(frequency), amplitude(amplitude), time(time) {}

    auto as_tuple() const {
        return std::make_tuple(damage, frequency, amplitude, time);
    }
};

struct PlayerShield
{
    uint32_t ownerId;

    PlayerShield(uint32_t ownerId = 0)
        : ownerId(ownerId) {}

    auto as_tuple() const {
        return std::make_tuple(ownerId);
    }
};

struct Healing
{
    int amout;
    auto as_tuple() const {
        return std::make_tuple(amout);
    }
};

struct Explosion
{
    float duration;

    Explosion(float duration = 0.0f)
        : duration(duration) {}

    auto as_tuple() const {
        return std::make_tuple(duration);
    }
};
