#ifndef CARDSLOTS_H
#define CARDSLOTS_H
#include "global.h"

class Card;

class CardSlots
{
    Q_OBJECT
public:
    explicit CardSlots(int slotNum,QObject* parent = nullptr);
    ~CardSlots();


private:
    int m_id;
    int m_slotNum;
    QStack<Card*> m_supplyPile;
    QList<QStack<Card*>> m_slots;
};

#endif // CARDSLOTS_H
