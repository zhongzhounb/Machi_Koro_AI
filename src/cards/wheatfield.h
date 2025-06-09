#ifndef WHEATFIELD_H
#define WHEATFIELD_H

#include "card.h"

class WheatField: public Card
{
public:
    explicit WheatField(QObject* parent=nullptr);
    ~WheatField();
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer, int rollSum, Game* game, const QVariant& choiceData) override;
};

#endif // WHEATFIELD_H
