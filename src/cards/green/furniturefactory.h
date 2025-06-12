#ifndef FURNITUREFACTORY_H
#define FURNITUREFACTORY_H
#include "cards/card.h"

class FurnitureFactory: public Card
{
public:
    explicit FurnitureFactory(QObject* parent=nullptr);
    ~FurnitureFactory(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // FURNITUREFACTORY_H
