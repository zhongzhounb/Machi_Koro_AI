#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include"global.h"

// 前向声明所有需要工厂创建的类
// 这样做可以避免在头文件中包含所有具体的命令类头文件，减少编译依赖
class GameCommand;
class Player;
class GameState; // 需要 GameState 来帮助恢复 Player*
class Card;

// 引入命令类型和优先级枚举
#include "gamecommand.h" // 包含 CommandType 和 EffectPriority

class CommandFactory {
public:
    // 单例模式，获取工厂实例
    static CommandFactory& instance();

    // 注册命令：将 CommandType 与一个创建该命令默认实例的函数关联起来。主要用于反序列化时，先创建空对象，再填充数据。
    template<typename T>
    void registerCommand(CommandType type) {
        m_creators[type] = []() { return new T(); };
    }

    // 1. 用于反序列化时创建命令：
    GameCommand* createCommandForDeserialization(CommandType type, QObject* parent = nullptr);

    // 2.用于运行时创建具体命令（带参数）：
    GameCommand* createGainCoinsCommands(Player* sourcePlayer,Card* card, QObject* parent = nullptr);

private:
    // 私有构造函数和删除拷贝构造/赋值运算符，确保单例模式
    CommandFactory() = default;
    CommandFactory(const CommandFactory&) = delete;
    CommandFactory& operator=(const CommandFactory&) = delete;

    // 存储命令类型到创建函数的映射
    QMap<CommandType, std::function<GameCommand*()>> m_creators;
};

#endif // COMMANDFACTORY_H
