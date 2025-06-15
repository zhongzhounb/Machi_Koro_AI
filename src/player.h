#ifndef PLAYER_H
#define PLAYER_H

#include "global.h"
#include "card.h"

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(int id, const QString& name, AIRank aiRank = AIRank::None, QObject* parent = nullptr);
    ~Player();

    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    int getCoins() const { return m_coins; }
    AIRank getAIRank() const { return m_aiRank; }
    QList<QList<Card*>> getCards() const { return m_cards; }
    // 获取某种状态的卡牌/地标数量（None为所有）
    int getCardNum(QString name,State state);
    // 获取某种类型的卡牌/地标数量（None为所有）
    int getTypeCardNum(Type type,State state);

    // 赚钱
    void addCoins(int amount);
    // 亏钱（不负责判负）
    void delCoins(int amount);
    // 从某个玩家偷钱（赚钱）
    void stealCoins(Player* player,int amount);
    // 添加卡牌
    void addCard(Card* card);
    // 移除卡牌
    void delCard(Card* card);
    // 设置卡牌/地标状态
    void setCardState(Card* card,State state);

signals:
    void coinsChange(int playId,int changeAmount);
    void coinsTransfer(int fromPlayId,int toPlayId,int amount);

protected:
    int m_id;
    QString m_name;
    AIRank m_aiRank;
    int m_coins;
    //实质是vector<stack<Card*>>，设计栈的目的是用户只能对第一个操作（如UI展示卡牌、每次最多移除一次
    //默认最顶部的卡），但是卡有状态量（比如装修），你需要对栈中元素操作，所以用List
    QList<QList<Card*>> m_cards;
};

#endif // PLAYER_H
