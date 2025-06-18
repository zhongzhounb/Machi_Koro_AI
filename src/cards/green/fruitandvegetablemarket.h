#ifndef FRUITANDVEGETABLEMARKET_H
#define FRUITANDVEGETABLEMARKET_H
#include "card.h"

class FruitAndVegetableMarket: public Card
{
public:
    explicit FruitAndVegetableMarket(QObject* parent=nullptr);
    ~FruitAndVegetableMarket(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // FRUITANDVEGETABLEMARKET_H
