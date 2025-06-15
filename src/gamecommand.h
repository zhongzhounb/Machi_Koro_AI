#ifndef GAMECOMMAND_H
#define GAMECOMMAND_H
#include "global.h"

class GameState;
class GameController;
class Player;
class Card;

class GameCommand : public QObject {
    Q_OBJECT
public:
    //传入Card和activePlayer只是用来计算优先级
    explicit GameCommand(CommandType type, Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                         Card* card=nullptr,Player* activePlayer=nullptr,bool isFailed=false,const QString& m_failureMessage="");
    virtual ~GameCommand() = default;

    CommandType getType() const { return m_type; }
    int getPriority() const { return m_priority; }
    Player* getSourcePlayer() const { return m_sourcePlayer; }
    QVariant getUserChoice() const { return m_userChoice; }
    QString getLog() const { return m_log; }

    // 检查是否需要用户交互（默认不需要交互）
    virtual bool requiresUserInput() const { return false; }

    // 【真人：前端→后端/AI：后端】设置选项 (由 GameController 或 AI 调用)
    void setChoice(const QVariant& choice) { m_userChoice = choice; }

    // 【真人：后端→前端】向 UI 发送请求信号。(在回放或AI玩家时不调用）
    virtual void prompt(GameController* controller) { Q_UNUSED(controller); }

    // 执行命令的核心逻辑。此方法假定 m_userChoice 已经设置。
    virtual void execute(GameState* state, GameController* controller) = 0;

    // 序列化和反序列化接口 (用于日志记录和回放)
    virtual QVariantMap serialize() const;
    virtual void deserialize(const QVariantMap& data, GameState* state);

protected:
    CommandType m_type;
    int m_priority;
    Player* m_sourcePlayer; // 触发此命令的玩家
    QVariant m_userChoice; // 存储用户/AI的选择，用于回放
    QString m_log;//日志
    bool m_isFailed;//【卡牌命令】是否激活后无效
    QString m_failureMessage;//【卡牌命令】无效原因
};


#endif // GAMECOMMAND_H
