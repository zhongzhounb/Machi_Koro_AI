#ifndef MOVINGCOMPANY_H
#define MOVINGCOMPANY_H
#include "card.h"

class MovingCompany: public Card
{
public:
    explicit MovingCompany(QObject* parent=nullptr);
    ~MovingCompany(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // MOVINGCOMPANY_H
