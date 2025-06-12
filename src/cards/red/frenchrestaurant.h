#ifndef FRENCHRESTAURANT_H
#define FRENCHRESTAURANT_H
#include "cards/card.h"

class FrenchRestaurant: public Card
{
public:
    explicit FrenchRestaurant(QObject* parent=nullptr);
    ~FrenchRestaurant(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
    bool isActivate (Player* owner, Player* activePlayer, int rollSum)const override;
};

#endif // FRENCHRESTAURANT_H
