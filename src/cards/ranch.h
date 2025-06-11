#ifndef RANCH_H
#define RANCH_H

#include "card.h"

class Ranch: public Card
{
public:
    explicit Ranch(QObject* parent=nullptr);
    ~Ranch(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // RANCH_H
