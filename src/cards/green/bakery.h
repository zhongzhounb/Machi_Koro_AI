#ifndef BAKERY_H
#define BAKERY_H
#include "card.h"

class Bakery: public Card
{
    Q_OBJECT
public:
    explicit Bakery(QObject* parent=nullptr);
    ~Bakery(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // BAKERY_H
