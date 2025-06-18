#ifndef PIZZAJOINT_H
#define PIZZAJOINT_H
#include "card.h"

class PizzaJoint: public Card
{
public:
    explicit PizzaJoint(QObject* parent=nullptr);
    ~PizzaJoint(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};
#endif // PIZZAJOINT_H
