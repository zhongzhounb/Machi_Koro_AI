#ifndef PIZZAJOINT_H
#define PIZZAJOINT_H
#include "cards/card.h"

class PizzaJoint: public Card
{
public:
    explicit PizzaJoint(QObject* parent=nullptr);
    ~PizzaJoint(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // PIZZAJOINT_H
