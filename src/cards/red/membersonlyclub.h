#ifndef MEMBERSONLYCLUB_H
#define MEMBERSONLYCLUB_H
#include "card.h"

class MembersOnlyClub: public Card
{
    Q_OBJECT
public:
    explicit MembersOnlyClub(QObject* parent=nullptr);
    ~MembersOnlyClub(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    QString getDescription() const override;
};

#endif // MEMBERSONLYCLUB_H
