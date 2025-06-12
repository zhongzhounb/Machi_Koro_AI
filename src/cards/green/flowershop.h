#ifndef FLOWERSHOP_H
#define FLOWERSHOP_H
#include "cards/card.h"

class FlowerShop: public Card
{
public:
    explicit FlowerShop(QObject* parent=nullptr);
    ~FlowerShop(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // FLOWERSHOP_H
