#ifndef CARDSTORE_H
#define CARDSTORE_H
#include "global.h"

class Card;
class Player;

class CardStore:public QObject
{
    Q_OBJECT
public:
    explicit CardStore(int id,int slotNum,Color type,QObject* parent = nullptr);
    ~CardStore();

    int getId() const { return m_id ;}
    Color getType() const { return m_type; }
    int getStoreSlotsCount() const { return m_slotNum; }
    QList<QList<Card*>> getSlots() const { return m_slots; }
    int getSupplyCount()const;

    //初始化添加供应堆
    void addCard(Card* card);
    //初始化打乱供应堆的顺序
    void shuffleCard();
    //查看卡槽是否有空
    bool hasEmptySlot()const;
    //将卡槽补满
    void suppleCard();
    //获取所有卡槽位中的栈顶卡
    QList<Card*> getCardFirst()const;
    //某张卡被买走（自动补充）
    void delCard(Card* card);
signals:
    //当供应堆增加卡牌时（一直是增加单张）
    void supplyCardAdded(CardStore* store);
    //当有新卡从供应堆拿出时，UI需要展示从供应堆拿出的动画，并放置第pos个卡槽
    void cardAdded(CardStore* store,Card* card,int pos);
    //当有卡被买走时
    void cardDeled(CardStore* store,Card*card,int pos);


private:
    int m_id;
    int m_slotNum;
    Color m_type;//显示供应堆的图片
    QList<Card*> m_supplyPile;
    //实质是vector<stack<Card*>>，因为在播放回放时可能会访问栈底内容，所以设计成List
    QList<QList<Card*>> m_slots;
    void addCardToSlot(Card* card,int pos);
};

#endif // CARDSTORE_H
