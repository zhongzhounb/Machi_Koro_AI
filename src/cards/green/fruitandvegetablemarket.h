#ifndef FRUITANDVEGETABLEMARKET_H
#define FRUITANDVEGETABLEMARKET_H
#include "card.h"

class FruitAndVegetableMarket: public Card
{
    Q_OBJECT
public:
    explicit FruitAndVegetableMarket(QObject* parent=nullptr);
    ~FruitAndVegetableMarket(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    QString getDescription() const override;
};

#endif // FRUITANDVEGETABLEMARKET_H
