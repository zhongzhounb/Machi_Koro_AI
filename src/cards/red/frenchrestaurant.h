#ifndef FRENCHRESTAURANT_H
#define FRENCHRESTAURANT_H
#include "card.h"

class FrenchRestaurant: public Card
{
    Q_OBJECT
public:
    explicit FrenchRestaurant(QObject* parent=nullptr);
    ~FrenchRestaurant(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    QString getDescription() const override;
};

#endif // FRENCHRESTAURANT_H
