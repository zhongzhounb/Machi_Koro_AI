#ifndef HARBOR_H
#define HARBOR_H
#include "card.h"

class Harbor: public Card
{
public:
    explicit Harbor(QObject* parent=nullptr);
    ~Harbor(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // HARBOR_H
