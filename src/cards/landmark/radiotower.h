#ifndef RADIOTOWER_H
#define RADIOTOWER_H
#include "card.h"

class RadioTower: public Card
{
public:
    explicit RadioTower(QObject* parent=nullptr);
    ~RadioTower(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    GameCommand* createSpecialCommand(Player* owner)override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // RADIOTOWER_H
