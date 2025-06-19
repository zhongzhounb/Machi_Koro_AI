#ifndef STATION_H
#define STATION_H
#include "card.h"

class Station: public Card
{
public:
    explicit Station(QObject* parent=nullptr);
    ~Station(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // STATION_H
