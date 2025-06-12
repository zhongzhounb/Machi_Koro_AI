#ifndef CAFE_H
#define CAFE_H
#include "cards/card.h"

class Cafe: public Card
{
public:
    explicit Cafe(QObject* parent=nullptr);
    ~Cafe(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};


#endif // CAFE_H
