#ifndef CHEESEFACTORY_H
#define CHEESEFACTORY_H
#include "cards/card.h"

class CheeseFactory: public Card
{
public:
    explicit CheeseFactory(QObject* parent=nullptr);
    ~CheeseFactory(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // CHEESEFACTORY_H
