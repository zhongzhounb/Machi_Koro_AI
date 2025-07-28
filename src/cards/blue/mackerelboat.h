#ifndef MACKERELBOAT_H
#define MACKERELBOAT_H
#include "card.h"

class MackerelBoat: public Card
{
    Q_OBJECT
public:
    explicit MackerelBoat(QObject* parent=nullptr);
    ~MackerelBoat(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // MACKERELBOAT_H
