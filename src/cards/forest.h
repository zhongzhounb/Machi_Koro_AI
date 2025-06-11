#ifndef FOREST_H
#define FOREST_H
#include "card.h"

class Forest: public Card
{
public:
    explicit Forest(QObject* parent=nullptr);
    ~Forest(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // FOREST_H
