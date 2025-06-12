#ifndef BAKERY_H
#define BAKERY_H
#include "cards/card.h"

class Bakery: public Card
{
public:
    explicit Bakery(QObject* parent=nullptr);
    ~Bakery(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // BAKERY_H
