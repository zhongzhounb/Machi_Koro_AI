#ifndef FLOWERSHOP_H
#define FLOWERSHOP_H
#include "card.h"

class FlowerShop: public Card
{
    Q_OBJECT
public:
    explicit FlowerShop(QObject* parent=nullptr);
    ~FlowerShop(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
private:
    QString m_comboName;
};
#endif // FLOWERSHOP_H
