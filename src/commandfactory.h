#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include"global.h"

// 前向声明所有需要工厂创建的类
// 这样做可以避免在头文件中包含所有具体的命令类头文件，减少编译依赖
class GameCommand;
class Player;
class GameState; // 需要 GameState 来帮助恢复 Player*
class Card;

class CommandFactory {
public:
    // 单例模式，获取工厂实例
    static CommandFactory& instance();

    GameCommand* createGainCoinsCommand(Player* sourcePlayer, Card* card, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
    GameCommand* createStealCoinsCommand(Player* sourcePlayer, Card* card,Player* activePlayer, QObject* parent=nullptr, bool isFailed=false, const QString& failureMessage="");
private:
    // 私有构造函数和删除拷贝构造/赋值运算符，确保单例模式
    CommandFactory() = default;
    CommandFactory(const CommandFactory&) = delete;
    CommandFactory& operator=(const CommandFactory&) = delete;
};

#endif // COMMANDFACTORY_H
