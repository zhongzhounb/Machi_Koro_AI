#ifndef MACKERELBOAT_H
#define MACKERELBOAT_H
#include "cards/card.h"

class MackerelBoat: public Card
{
public:
    explicit MackerelBoat(QObject* parent=nullptr);
    ~MackerelBoat(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
    bool isActivate (Player* owner, Player* activePlayer, int rollSum)const override;
};

#endif // MACKERELBOAT_H
