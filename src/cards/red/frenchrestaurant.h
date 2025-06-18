#ifndef FRENCHRESTAURANT_H
#define FRENCHRESTAURANT_H
#include "card.h"

class FrenchRestaurant: public Card
{
public:
    explicit FrenchRestaurant(QObject* parent=nullptr);
    ~FrenchRestaurant(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // FRENCHRESTAURANT_H
