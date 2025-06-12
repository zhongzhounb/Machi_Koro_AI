#ifndef STADIUM_H
#define STADIUM_H
#include "cards/card.h"

class Stadium: public Card
{
public:
    explicit Stadium(QObject* parent=nullptr);
    ~Stadium(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // STADIUM_H
