#include "Tank/GameLoop.h"

void GameLoop::handleOnOpen(TcpConnection *tc)
{
}

void GameLoop::handleOnClose(TcpConnection *tc)
{
}

void GameLoop::handleOnMessage(const std::string msg, TcpConnection *tc)
{

     webSocketServer.send(msg, tc);
     return;
     if (msg.empty())
          return;
     MessageType messageType = static_cast<MessageType>(msg[0]);
     if (messageType == MessageType::PlayerInteractionInfo)
     {
          OperationType operationType = static_cast<OperationType>(msg[1]);
          if (operationType == OperationType::MouseMove)
          {
               float tmpangle;
               std::memcpy(&tmpangle, msg.data() + 2, 4);
               if (players.find(tc) != players.end())
                    players[tc]->aim(tmpangle);
          }
          else if (operationType == OperationType::MouseDown)
          {
          }
          else if (operationType == OperationType::KeyDown)
          {
               std::shared_lock<std::shared_mutex> lock(playersSharedMutex); // 读
               if (players.find(tc) != players.end())
                    players[tc]->keyDown(msg.substr(2));
          }
          else if (operationType == OperationType::KeyUp)
          {
               std::shared_lock<std::shared_mutex> lock(playersSharedMutex); // 读
               if (players.find(tc) != players.end())
                    players[tc]->keyUp(msg.substr(2));
          }
          else if (operationType == OperationType::CreateCharacter)
          {
               std::unique_lock<std::shared_mutex> lock(playersSharedMutex); // 写
               players[tc] = world.addPlayer();
               // 返回创建的角色ID
               std::string strData;
               strData.push_back((uint8_t)0x01);
               strData.push_back((uint8_t)0x03);
               uint64_t ID = b2StoreBodyId(players[tc]->getBodyId());
               char tmpCh[8];
               std::memcpy(tmpCh, &ID, 8);
               strData += std::string(tmpCh, 8);
               webSocketServer.send(strData, tc);
               Logger::instance().info("创建角色成功");
               std::cout << "palyer num: " << players.size() << std::endl;
          }
          else
          {
          }
     }
     else if (messageType == MessageType::Ping)
     {
          webSocketServer.send(msg, tc);
     }
}

GameLoop::GameLoop() : world(10, 10, (b2Vec2){0.f, 0.f}),
                       webSocketServer(InetAddress("0.0.0.0", 7792))
{
     webSocketServer.setOnOpen(std::bind(&GameLoop::handleOnOpen, this, std::placeholders::_1));
     webSocketServer.setOnMessage(std::bind(&GameLoop::handleOnMessage, this, std::placeholders::_1, std::placeholders::_2));
     webSocketServer.setOnclose(std::bind(&GameLoop::handleOnClose, this, std::placeholders::_1));
}

void GameLoop::loop()
{
     std::thread WebSocketServerThread(&WebSocketServer::run, std::ref(webSocketServer));
     while (true)
     {
          {
               std::shared_lock<std::shared_mutex> lock(playersSharedMutex); // 读
               for (auto &v : players)
                    v.second->fixedUpdate();
          }
          world.updateWorld();
          std::this_thread::sleep_for(std::chrono::duration<double>(world.getTimeStep()));
          {
               std::unique_lock<std::shared_mutex> lock(playersSharedMutex); // 写
               for (auto it = players.begin(); it != players.end();)
               {
                    bool ok = webSocketServer.send((*it).second->getFrameData(), (*it).first);
                    if (ok)
                         ++it;
                    else
                    {
                         world.removeBody((*it).second);
                         it = players.erase(it);
                         Logger::instance().info("销毁成功");
                         std::cout << "palyer num: " << players.size() << std::endl;
                    }
               }
          }
     }
}