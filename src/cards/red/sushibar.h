#ifndef SUSHIBAR_H
#define SUSHIBAR_H
#include "cards/card.h"

class SushiBar: public Card
{
public:
    explicit SushiBar(QObject* parent=nullptr);
    ~SushiBar(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
    bool isActivate (Player* owner, Player* activePlayer, int rollSum)const override;
};


#endif // SUSHIBAR_H
