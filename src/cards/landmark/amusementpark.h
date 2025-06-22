#ifndef AMUSEMENTPARK_H
#define AMUSEMENTPARK_H
#include "card.h"

class AmusementPark: public Card
{
public:
    explicit AmusementPark(QObject* parent=nullptr);
    ~AmusementPark(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    GameCommand* createSpecialCommand(Player* owner)override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // AMUSEMENTPARK_H
