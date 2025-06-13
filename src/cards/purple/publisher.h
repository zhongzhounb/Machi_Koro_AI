#ifndef PUBLISHER_H
#define PUBLISHER_H
#include "cards/card.h"

class Publisher: public Card
{
public:
    explicit Publisher(QObject* parent=nullptr);
    ~Publisher(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // PUBLISHER_H
