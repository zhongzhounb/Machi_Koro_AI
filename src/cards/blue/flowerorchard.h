#ifndef FLOWERORCHARD_H
#define FLOWERORCHARD_H
#include "cards/card.h"

class FlowerOrchard: public Card
{
public:
    explicit FlowerOrchard(QObject* parent=nullptr);
    ~FlowerOrchard(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};
#endif // FLOWERORCHARD_H
