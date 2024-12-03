#pragma once
#include "GameObject.h"
class Player : public GameObject
{
private:
     int gold;               // 金币
     int score;              // 积分
     float size;             // 大小(半径单位米)
     float moveForce = 0.1f; // 单位 N

public:
     bool keyStates[4] = {0, 0, 0, 0}; // w a s d是否按下
     Player(int health, float size, b2BodyId bodyId, std::mutex &worldMutex);

     int getGold();
     int getScore();
     void setGold(int gold);
     void setScore(int score);

     // 按键按下
     void keyDown(char key);

     // 按键抬起
     void keyUp(char key);

     void fixedUpdate() override;

     std::string packData() const override;
};