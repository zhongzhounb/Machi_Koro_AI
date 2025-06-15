#include "commandfactory.h"
#include "gamecommand.h"
#include "commands/gaincoinscommand.h" // 确保包含 GainCoinsCommand 的头文件

CommandFactory& CommandFactory::instance() {
    static CommandFactory factory;
    return factory;
}

GameCommand* CommandFactory::createGainCoinsCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage) {
    return new GainCoinsCommand(sourcePlayer, card, parent, isFailed, failureMessage);
}

// 在 main.cpp 或某个初始化函数中注册所有命令
void registerAllCommands() {
    CommandFactory::instance().registerCommand<GainCoinsCommand>(CommandType::GainCoins);
      // ... 注册所有命令
}
