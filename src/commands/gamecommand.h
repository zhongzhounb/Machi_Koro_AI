// gamecommand.h
#ifndef GAMECOMMAND_H
#define GAMECOMMAND_H
#include"global.h"

class Player;
class Game; // 前向声明

class GameCommand : public QObject
{
    Q_OBJECT
public:
    GameCommand(Player* owner, Game* game, QObject* parent = nullptr);
    virtual ~GameCommand() = default;

    Player* getOwner() const { return m_owner; }
    CommandStatus getStatus() const { return m_status; }
    void setStatus(CommandStatus status) { m_status = status; }

    // 核心：执行命令的虚函数
    // 返回 true 表示命令已完成，可以执行下一个命令
    // 返回 false 表示命令正在执行中，需要等待外部输入 (例如玩家选择)，Game 将暂停队列
    virtual bool execute() = 0;

    // 如果命令需要玩家选择，此函数返回选项列表
    virtual QVariantList getChoices() const { return {}; }

    // 如果命令需要玩家选择，此函数接收选择结果并继续执行
    virtual void processChoice(const QVariant& choiceData) { Q_UNUSED(choiceData); }

    // 用于调试和日志
    virtual QString toString() const = 0;

signals:
    // 命令可能需要发出信号来请求UI交互
    void requestPlayerChoice(Player* chooser, const QString& prompt, const QVariantList& choices);
    // ... 其他UI交互信号 ...

protected:
    QPointer<Player> m_owner; // 命令的发出者或主要作用者
    QPointer<Game> m_game;    // 游戏实例引用
    CommandStatus m_status;
};

#endif // GAMECOMMAND_H
