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

    int getId() const { return m_id; }
    CommandType getType() const { return m_type; }
    int getPriority() const { return m_priority; }
    Player* getSourcePlayer() const { return m_sourcePlayer; }

    // 获得选项（单选，多选也要分成单选，即选择后都由后端处理，为了处理某些卡选择后其他卡某些卡不能选的问题），如果没有则不调用
    virtual PromptData getPromptData(GameState* state) const { Q_UNUSED(state);return {PromptData::None, "", {}}; }

    // 获取默认选项（无选项时禁止调用）
    virtual int getAutoInput( const PromptData& promptData ,GameState* state) const{ Q_UNUSED(promptData);Q_UNUSED(state);return 0; }

    // 设置选项，返回是否要继续获得选项（无选项时禁止调用）
    virtual bool setInput(int optionId,GameState* state) {
        Q_UNUSED(optionId);
        Q_UNUSED(state);
        Q_ASSERT(false && "GainMoneyCommand::processUserInput should not be called as it's a non-interactive command."); return true;}

    // 执行命令的核心逻辑。此方法假定 m_userChoice 已经设置。
    virtual void execute(GameState* state, GameController* controller=nullptr) = 0;

    // 获取日志
    virtual QString getLog() const=0;

protected:
    int m_id;
    CommandType m_type;
    int m_priority;
    Player* m_sourcePlayer; // 触发此命令的玩家
    Card* m_card;//【卡牌命令】
    bool m_isFailed;//【卡牌命令】是否激活后无效
    Player* m_activePlayer;//【卡牌命令】激活者（大部分不用传入）
    QString m_failureMessage;//【卡牌命令】无效原因
    int m_currentStep;//当前步骤
    QList<int> m_userInput;//有输入的命令用户输入


private:
    static int s_commandId;
};


#endif // GAMECOMMAND_H
