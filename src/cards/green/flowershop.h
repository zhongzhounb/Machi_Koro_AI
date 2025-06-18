#ifndef FLOWERSHOP_H
#define FLOWERSHOP_H
#include "card.h"

class FlowerShop: public Card
{
public:
    explicit FlowerShop(QObject* parent=nullptr);
    ~FlowerShop(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};
#endif // FLOWERSHOP_H
