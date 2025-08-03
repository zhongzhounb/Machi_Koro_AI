#ifndef FURNITUREFACTORY_H
#define FURNITUREFACTORY_H
#include "card.h"

class FurnitureFactory: public Card
{
    Q_OBJECT
public:
    explicit FurnitureFactory(QObject* parent=nullptr);
    ~FurnitureFactory(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    QString getDescription() const override;
};

#endif // FURNITUREFACTORY_H
