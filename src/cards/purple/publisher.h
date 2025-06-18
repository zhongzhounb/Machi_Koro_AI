#ifndef PUBLISHER_H
#define PUBLISHER_H
#include "card.h"

class Publisher: public Card
{
public:
    explicit Publisher(QObject* parent=nullptr);
    ~Publisher(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // PUBLISHER_H
