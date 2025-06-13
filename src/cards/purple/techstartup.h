#ifndef TECHSTARTUP_H
#define TECHSTARTUP_H
#include "cards/card.h"

class TechStartup: public Card
{
public:
    explicit TechStartup(QObject* parent=nullptr);
    ~TechStartup(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // TECHSTARTUP_H
