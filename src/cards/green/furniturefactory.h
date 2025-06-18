#ifndef FURNITUREFACTORY_H
#define FURNITUREFACTORY_H
#include "card.h"

class FurnitureFactory: public Card
{
public:
    explicit FurnitureFactory(QObject* parent=nullptr);
    ~FurnitureFactory(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // FURNITUREFACTORY_H
