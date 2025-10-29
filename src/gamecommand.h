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
                         QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~GameCommand() = default;

    int getId() const { return m_id; }
    CommandType getType() const { return m_type; }
    int getPriority() const { return m_priority; }
    Player* getSourcePlayer() const { return m_sourcePlayer; }
    Card* getCard(){return m_card;}

    // 获得选项（单选，多选也要分成单选，即选择后都由后端处理，为了处理某些卡选择后其他卡某些卡不能选的问题），如果没有则不调用
    virtual PromptData getPromptData(GameState* state) const;
    // 设置选项
    virtual bool setInput(int optionId,GameState* state,GameController* controller);
    // 执行命令的核心逻辑。此方法假定 m_userChoice 已经设置。为了方便存储计算结果，就不存储，直接输出日志。
    virtual void execute(GameState* state, GameController* controller=nullptr) = 0;

protected:
    int m_id;
    CommandType m_type;
    int m_priority;
    Player* m_sourcePlayer; // 触发此命令的玩家
    QList<Card*> m_cards;//【卡牌命令】
    Player* m_activePlayer;//【卡牌命令】激活者（大部分不用传入）
    int m_currentStep=1;//当前步骤（有主动的命令）（都是从第一步开始）
    QList<int> m_userInput;//有输入的命令用户输入（回放用）
    Card* m_card;//（todo：由于目前的非地标卡牌没有加入装修状态以及收益不同（即某人的一种卡中，除了ID不同其余相同）
    //目前command是通过操作m_cards.last()简单乘算，后期如果出现变化需要再次重构。
    //辅助函数，快速增删命令
    void delCommand(QString name,GameController* controller);
    void addCommand(QString name,GameController* controller);

private:
    static int s_commandId;
};


#endif // GAMECOMMAND_H
