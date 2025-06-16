#ifndef MACKERELBOAT_H
#define MACKERELBOAT_H
#include "card.h"

class MackerelBoat: public Card
{
public:
    explicit MackerelBoat(QObject* parent=nullptr);
    ~MackerelBoat(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // MACKERELBOAT_H
