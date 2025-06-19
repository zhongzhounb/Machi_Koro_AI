#ifndef SHOPPINGMALL_H
#define SHOPPINGMALL_H
#include "card.h"

class ShoppingMall: public Card
{
public:
    explicit ShoppingMall(QObject* parent=nullptr);
    ~ShoppingMall(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // SHOPPINGMALL_H
