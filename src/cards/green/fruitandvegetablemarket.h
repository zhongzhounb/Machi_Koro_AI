#ifndef FRUITANDVEGETABLEMARKET_H
#define FRUITANDVEGETABLEMARKET_H
#include "cards/card.h"

class FruitAndVegetableMarket: public Card
{
public:
    explicit FruitAndVegetableMarket(QObject* parent=nullptr);
    ~FruitAndVegetableMarket(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // FRUITANDVEGETABLEMARKET_H
