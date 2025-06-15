#ifndef FOREST_H
#define FOREST_H
#include "card.h"

class Forest: public Card
{
public:
    explicit Forest(QObject* parent=nullptr);
    ~Forest(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer,GameController* controller) override;
    QString getDescription() const override;
};

#endif // FOREST_H
