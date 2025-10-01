#ifndef PLAYER_H
#define PLAYER_H

#include "global.h"
class Card;
class GameCommand;

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(const QString& name, AIRank aiRank = AIRank::None, QObject* parent = nullptr);
    ~Player();

    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    int getCoins() const { return m_coins; }
    AIRank getAIRank() const { return m_aiRank; }
    QList<QList<Card*>> getCards() const { return m_cards; }
    QList<Card*> getCardsForName(QString name);
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

signals:
    void coinsChange(Player* player,int amount,int change);
    // 从某个玩家偷钱（赚钱）
    void coinsMoved(Player* fromPlayer,Player* toPlayer,int amount);
    // 添加卡牌
    void cardAdded(Player* player,Card* card);
    // 移除卡牌
    void cardDeled(Player* player,Card* card);

protected:
    int m_id;
    QString m_name;
    AIRank m_aiRank;
    int m_coins;
    //实质是vector<stack<Card*>>，设计栈的目的是用户只能对第一个操作（如UI展示卡牌、每次最多移除一次
    //默认最顶部的卡），但是卡有状态量（比如装修），你需要对栈中元素操作，所以用List
    QList<QList<Card*>> m_cards;

private:
    static int s_playerId;
};

#endif // PLAYER_H
