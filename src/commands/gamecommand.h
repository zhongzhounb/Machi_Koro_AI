#ifndef GAMECOMMAND_H
#define GAMECOMMAND_H
#include "global.h"

class GameState;
class Game;
class Player;

class GameCommand : public QObject {
    Q_OBJECT
public:
    explicit GameCommand(CommandType type, int priority, Player* sourcePlayer = nullptr, QObject* parent = nullptr);
    virtual ~GameCommand() = default;

    CommandType getType() const { return m_type; }
    int getPriority() const { return m_priority; }
    Player* getSourcePlayer() const { return m_sourcePlayer; }
    QVariant getUserChoice() const { return m_userChoice; }

    // 检查是否需要用户交互（默认不需要交互）
    virtual bool requiresUserInput() const { return false; }

    // （如果要交互）设置用户选择 (由 GameController 或 AI 调用)
    void setUserChoice(const QVariant& choice) { m_userChoice = choice; }

    // 执行命令的核心逻辑。此方法假定 m_userChoice 已经设置（如果 requiresUserInput() 为 true）。
    virtual void execute(GameState* state, Game* game) = 0;

    // 对于需要用户输入的命令，此方法用于向 UI 发送请求信号。(在回放或AI玩家时不调用）
    virtual void prompt(Game* game) { Q_UNUSED(game); }

    // 序列化和反序列化接口 (用于日志记录和回放)
    virtual QVariantMap serialize() const;
    virtual void deserialize(const QVariantMap& data, GameState* state);

protected:
    CommandType m_type;
    int m_priority;
    Player* m_sourcePlayer; // 触发此命令的玩家
    QVariant m_userChoice; // 存储用户/AI的选择，用于回放
};


#endif // GAMECOMMAND_H
