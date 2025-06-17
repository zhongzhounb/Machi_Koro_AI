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
    explicit GameCommand(CommandType type, Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                         Card* card=nullptr,Player* activePlayer=nullptr,bool isFailed=false,const QString& m_failureMessage="");
    virtual ~GameCommand() = default;

    CommandType getType() const { return m_type; }
    int getPriority() const { return m_priority; }
    Player* getSourcePlayer() const { return m_sourcePlayer; }
    QVariant getUserChoice() const { return m_userChoice; }

    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const { return false; }

    // 获得选项
    virtual QVariantMap getPromptData() const { return QVariantMap(); };

    // 获取默认选项
    virtual QVariantMap getAutoChoice(const QVariantMap& promptData);

    // 设置选项
    void setChoice(const QVariantMap& choice) { m_userChoice = choice; }


    // 执行命令的核心逻辑。此方法假定 m_userChoice 已经设置。
    virtual void execute(GameState* state, GameController* controller) = 0;

    // 获取日志
    virtual QString getLog() const=0;

protected:
    int m_id;
    CommandType m_type;
    int m_priority;
    Player* m_sourcePlayer; // 触发此命令的玩家
    QVariantMap m_userChoice; // 存储用户/AI的选择，用于回放
    Card* m_card;//【卡牌命令】
    bool m_isFailed;//【卡牌命令】是否激活后无效
    Player* m_activePlayer;//【卡牌命令】激活者（大部分不用传入）
    QString m_failureMessage;//【卡牌命令】无效原因

private:
    static int s_commandId;
};


#endif // GAMECOMMAND_H
