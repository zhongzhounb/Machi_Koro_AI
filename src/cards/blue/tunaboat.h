#ifndef TUNABOAT_H
#define TUNABOAT_H
#include "card.h"

class TunaBoat: public Card
{
    Q_OBJECT
public:
    explicit TunaBoat(QObject* parent=nullptr);
    ~TunaBoat(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // TUNABOAT_H
