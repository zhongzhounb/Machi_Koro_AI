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
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // MEMBERSONLYCLUB_H
