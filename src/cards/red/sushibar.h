#ifndef SUSHIBAR_H
#define SUSHIBAR_H
#include "card.h"

class SushiBar: public Card
{
    Q_OBJECT
public:
    explicit SushiBar(QObject* parent=nullptr);
    ~SushiBar(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};


#endif // SUSHIBAR_H
