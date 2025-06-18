#ifndef CHEESEFACTORY_H
#define CHEESEFACTORY_H
#include "card.h"

class CheeseFactory: public Card
{
public:
    explicit CheeseFactory(QObject* parent=nullptr);
    ~CheeseFactory(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // CHEESEFACTORY_H
