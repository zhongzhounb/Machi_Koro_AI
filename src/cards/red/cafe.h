#ifndef CAFE_H
#define CAFE_H
#include "card.h"

class Cafe: public Card
{
    Q_OBJECT
public:
    explicit Cafe(QObject* parent=nullptr);
    ~Cafe(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};


#endif // CAFE_H
