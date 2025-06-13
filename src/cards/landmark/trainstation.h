#ifndef TRAINSTATION_H
#define TRAINSTATION_H
#include "cards/card.h"

class TrainStation: public Card
{
public:
    explicit TrainStation(QObject* parent=nullptr);
    ~TrainStation(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // TRAINSTATION_H
