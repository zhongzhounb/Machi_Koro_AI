#ifndef PLAYER_H
#define PLAYER_H

#include "global.h"
#include "cards/card.h"

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
    QList<Card*>getlandmarks() const { return m_landmarks; }
    QList<QList<Card*>> getCards() const { return m_cards; }

    //初始化地标建筑
    void addLandmark(Card* landmark);
    // 赚钱
    void addCoins(int amount);
    // 亏钱（不负责判负）
    void delCoins(int amount);
    // 添加卡牌
    void addCard(Card* card);
    // 移除卡牌
    void delCard(Card* card);
    // 设置卡牌/地标状态
    void setCardState(Card* card,State state);
    // 查看地标建筑是否激活
    bool isLandmarkOpening(QString name);

protected:
    int m_id;
    QString m_name;
    AIRank m_aiRank;
    int m_coins;
    //实质是vector<stack<Card*>>，设计栈的目的是用户只能对第一个操作（如UI展示卡牌、每次最多移除一次
    //默认最顶部的卡），但是卡有状态量（比如装修），你需要对栈中元素操作，所以用List
    QList<QList<Card*>> m_cards;
    QList<Card*> m_landmarks;
};

#endif // PLAYER_H
