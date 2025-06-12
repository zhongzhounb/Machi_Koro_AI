#ifndef FAMILYRESTAURANT_H
#define FAMILYRESTAURANT_H
#include "cards/card.h"

class FamilyRestaurant: public Card
{
public:
    explicit FamilyRestaurant(QObject* parent=nullptr);
    ~FamilyRestaurant(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // FAMILYRESTAURANT_H
