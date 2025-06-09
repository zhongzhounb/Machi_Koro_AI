#ifndef CARDSTORE_H
#define CARDSTORE_H
#include "global.h"

class Card;

class CardStore:public QObject
{
    Q_OBJECT
public:
    explicit CardStore(int id,int slotNum,QObject* parent = nullptr);
    ~CardStore();

    int getId() const { return m_id ;}
    int getSlotNum() const { return m_slotNum; }

    //初始化添加供应堆
    void addCard(Card* card);
    //初始化打乱供应堆的顺序
    void shuffleCard();
    //查看卡槽是否有空
    bool hasEmptySlot()const;
    //供应堆补充一张卡牌至卡槽
    Card* suppleCard();
    //获取所有卡槽位中的栈顶卡
    QList<Card*> getCardFirst()const;
    //某张卡被买走
    void delCard(Card* card);


private:
    int m_id;
    int m_slotNum;
    QStack<Card*> m_supplyPile;
    //实质是vector<stack<Card*>>，因为只有顶部可能被买，而底部无需操作，所以设计栈
    QList<QStack<Card*>> m_slots;
};

#endif // CARDSTORE_H
