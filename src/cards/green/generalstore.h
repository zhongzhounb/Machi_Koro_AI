#ifndef GENERALSTORE_H
#define GENERALSTORE_H
#include "cards/card.h"

class GeneralStore: public Card
{
public:
    explicit GeneralStore(QObject* parent=nullptr);
    ~GeneralStore(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
    bool isActivate (Player* owner, Player* activePlayer, int rollSum)const override;
};

#endif // GENERALSTORE_H
