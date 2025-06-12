#ifndef APPLEORCHARD_H
#define APPLEORCHARD_H
#include "cards/card.h"

class AppleOrchard: public Card
{

public:
    explicit AppleOrchard(QObject* parent=nullptr);
    ~AppleOrchard(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // APPLEORCHARD_H
