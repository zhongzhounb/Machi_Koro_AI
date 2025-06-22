#ifndef CARDSTORE_H
#define CARDSTORE_H
#include "global.h"

class Card;

class CardStore:public QObject
{
    Q_OBJECT
public:
    explicit CardStore(int id,int slotNum,QString name,QObject* parent = nullptr);
    ~CardStore();

    int getId() const { return m_id ;}
    int getSlotNum() const { return m_slotNum; }
    QList<QList<Card*>> getSlots() const { return m_slots; }

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


private:
    int m_id;
    int m_slotNum;
    QString m_name;
    QList<Card*> m_supplyPile;
    //实质是vector<stack<Card*>>，因为在播放回放时可能会访问栈底内容，所以设计成List
    QList<QList<Card*>> m_slots;
    void addCardToSlot(Card* card,int pos);
};

#endif // CARDSTORE_H
