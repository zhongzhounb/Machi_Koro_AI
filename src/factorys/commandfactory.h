#ifndef COMMANDFACTORY_H
#define COMMANDFACTORY_H

#include "gamecommand.h" // Base class GameCommand
#include <QDebug> // For logging warnings

// 包含所有具体命令类的头文件
// 请确保这些路径和文件名与你的项目结构完全匹配




/**
 * @brief The CommandFactory class is responsible for creating instances of GameCommand
 *        subclasses based on a given CommandType.
 *        It implements the Singleton design pattern.
 */
class CommandFactory {
private:
    // 私有构造函数，确保只能通过 instance() 获取实例
    CommandFactory();

    // 禁用拷贝构造函数和赋值运算符，防止复制
    CommandFactory(const CommandFactory&) = delete;
    CommandFactory& operator=(const CommandFactory&) = delete;

public:
    /**
     * @brief 获取 CommandFactory 的单例实例。
     * @return CommandFactory 的唯一实例引用。
     */
    static CommandFactory& instance();

    /**
     * @brief 创建并返回一个指定类型的 GameCommand 对象。
     * @param type CommandType 枚举值，指示要创建的命令类型。
     * @param sourcePlayer 触发此命令的玩家（可选）。
     * @param parent QObject 父对象，用于内存管理（可选）。
     * @param card 与命令关联的卡牌（对于卡牌相关命令，可选）。
     * @param activePlayer 命令的激活者（对于卡牌相关命令，可选）。
     * @return 指向新创建的 GameCommand 对象的指针，如果类型未知或为 None 则返回 nullptr。
     *         调用者负责管理返回对象的内存。
     */
    GameCommand* createCommand(CommandType type, Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                               Card* card = nullptr, Player* activePlayer = nullptr);
};

#endif // COMMANDFACTORY_H
