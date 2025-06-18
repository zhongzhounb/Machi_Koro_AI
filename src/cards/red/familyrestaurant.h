#ifndef FAMILYRESTAURANT_H
#define FAMILYRESTAURANT_H
#include "card.h"

class FamilyRestaurant: public Card
{
public:
    explicit FamilyRestaurant(QObject* parent=nullptr);
    ~FamilyRestaurant(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // FAMILYRESTAURANT_H
