#ifndef PLAYER_H
#define PLAYER_H

#include "global.h"
#include "cards/card.h" // 包含 Card 类定义，以便使用 Card::CardState

class Player : public QObject
{
    Q_OBJECT
public:
    Player(int id, const QString& name, AIRank aiRank = AIRank::None, QObject* parent = nullptr);
    ~Player();

    int getId() const { return m_id; }
    QString getName() const { return m_name; }
    int getCoins() const { return m_coins; }
    AIRank getAIRank() const { return m_aiRank; }
    QList<Card*> getCards() const { return m_cards; }

    void addCoins(int amount);
    void removeCoins(int amount);
    // 添加卡牌实例 (购买时 new 出来)
    void addCard(Card* cardInstance);
    // 移除卡牌实例 (例如被拆除、交换)
    void removeCard(Card* cardInstance);
    // 获取某个卡牌类型的数量
    int getCardTypeCount(int cardId) const;
    // 获取某个卡牌类型的特定状态数量 (用于“装修”等状态)
    int getCardTypeStateCount(int cardId, State state) const;
    // 获取特定卡牌类型的实例列表 (例如，获取所有麦田实例)
    QList<Card*> getCardInstancesByType(int cardId) const;

signals:
    void coinsChanged(int newCoins);
    void cardAdded(Card* cardInstance);
    void cardRemoved(Card* cardInstance);


protected:
    int m_id;
    QString m_name;
    AIRank m_aiRank;
    int m_coins;
    QList<Card*> m_cards;
};

#endif // PLAYER_H
