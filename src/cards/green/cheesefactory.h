#ifndef CHEESEFACTORY_H
#define CHEESEFACTORY_H
#include "card.h"

class CheeseFactory: public Card
{
    Q_OBJECT
public:
    explicit CheeseFactory(QObject* parent=nullptr);
    ~CheeseFactory(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    QString getDescription() const override;
};

#endif // CHEESEFACTORY_H
