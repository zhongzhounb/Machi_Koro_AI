#ifndef SODABOTTLINGPLANT_H
#define SODABOTTLINGPLANT_H
#include "cards/card.h"

class SodaBottlingPlant: public Card
{
public:
    explicit SodaBottlingPlant(QObject* parent=nullptr);
    ~SodaBottlingPlant(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // SODABOTTLINGPLANT_H
