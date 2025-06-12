#ifndef MEMBERSONLYCLUB_H
#define MEMBERSONLYCLUB_H
#include "cards/card.h"

class MembersOnlyClub: public Card
{
public:
    explicit MembersOnlyClub(QObject* parent=nullptr);
    ~MembersOnlyClub(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
    bool isActivate (Player* owner, Player* activePlayer, int rollSum)const override;
};

#endif // MEMBERSONLYCLUB_H
