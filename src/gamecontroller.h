#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "global.h"     // 包含 CommandType, AIRank 等全局定义
#include "gamecommand.h" // 包含 GameCommand 基类
#include "gamestate.h"   // 包含 GameState 类
#include "player.h"      // 包含 Player 类

/**
 * @brief GameController 类是游戏的核心控制器，负责管理游戏状态、命令队列和玩家交互。
 *        它协调命令的执行，处理用户输入和AI决策，并通知UI更新。
 */
class GameController : public QObject {
    Q_OBJECT // 启用Qt的元对象系统特性（信号、槽、属性等）

public:
    /**
     * @brief 构造函数
     * @param parent QObject 父对象
     */
    explicit GameController(QObject* parent = nullptr);

    /**
     * @brief 析构函数
     *        负责清理未处理的命令和游戏状态对象。
     */
    ~GameController();

    /**
     * @brief 将一个命令添加到命令队列中。
     *        一旦命令被添加，控制器会尝试处理它。
     * @param command 要添加的 GameCommand 指针。控制器将拥有此对象的生命周期。
     */
    void addCommand(GameCommand* command);

    /**
     * @brief 处理命令队列中的下一个命令。
     *        这是游戏流程的核心循环，它会根据命令类型和玩家类型决定如何处理命令。
     *        如果命令需要用户输入，它会发出信号给UI并暂停处理，直到收到用户选择。
     */
    void processNextCommand();

signals:
    /**
     * @brief 信号：请求UI显示用户输入界面。
     *        当一个需要人类玩家输入的命令被处理时发出。
     * @param promptData 一个 QVariantMap，包含UI显示提示所需的所有信息（如命令ID、消息、选项等）。
     */
    void requestUserInputPrompt(const QVariantMap& promptData);

    /**
     * @brief 信号：发送游戏日志消息给UI或其他日志记录组件。
     * @param message 要记录的日志字符串。
     */
    void logMessage(const QString& message);

public slots:
    /**
     * @brief 槽：接收UI回传的用户选择。
     *        UI在用户做出选择后调用此槽，将选择结果传递给控制器。
     * @param commandId 对应于之前 requestUserInputPrompt 信号中发送的命令ID。
     * @param choice 一个 QVariantMap，封装了用户做出的选择。
     */
    void handleUserChoice(int commandId, const QVariantMap& choice);

    /**
     * @brief 槽：由 GameCommand::execute (或AI处理后) 调用，通知控制器命令已完成执行。
     *        此槽负责清理已完成的命令对象，并触发处理命令队列中的下一个命令。
     * @param command 已完成执行的 GameCommand 指针。
     */
    void onCommandFinished(GameCommand* command);

private:
    GameState* m_state;               ///< 游戏状态对象，管理所有游戏数据
    QList<GameCommand*> m_commandsQueue;   ///< 存储待执行命令的队列
    GameCommand* m_currentCommand;         ///< 当前正在处理或等待用户输入的命令
};

#endif // GAMECONTROLLER_H
