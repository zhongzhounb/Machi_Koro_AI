#ifndef AIRPORT_H
#define AIRPORT_H
#include "card.h"

class Airport: public Card
{
    Q_OBJECT
public:
    explicit Airport(QObject* parent=nullptr);
    ~Airport(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    GameCommand* createSpecialCommand(Player* owner)override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // AIRPORT_H
