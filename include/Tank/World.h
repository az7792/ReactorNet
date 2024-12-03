#pragma once
#include "GameObject.h"
#include "Player.h"
#include "Logger.h"
#include <deque>
#include <mutex>
class World
{
private:
     b2WorldId worldId;
     std::mutex worldMutex;       // 保护世界状态的锁
     float timeStep = 1.f / 60.f; // 帧间隔

public:
     World(b2Vec2 gravity);
     World(float w, float h, b2Vec2 gravity); // 创建一个2w * 2h的世界(单位为米)
     ~World();

     // 禁用移动和拷贝
     World(const World &) = delete;
     World(World &&) = delete;
     World &operator=(const World &) = delete;
     World &operator=(World &&) = delete;

     // 获取世界id
     b2WorldId getWorldId();

     // 获取帧间隔
     float getTimeStep();

     // 更新一次世界
     void updateWorld();

     // 增加一个玩家
     Player *addPlayer();

     // 删除一个物体
     void removeBody(GameObject *obj);
};