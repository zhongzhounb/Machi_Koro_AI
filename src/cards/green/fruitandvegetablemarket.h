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
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
private:
    Type m_comboType;
};

#endif // FRUITANDVEGETABLEMARKET_H
