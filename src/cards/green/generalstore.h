#ifndef GENERALSTORE_H
#define GENERALSTORE_H
#include "card.h"

class GeneralStore: public Card
{
public:
    explicit GeneralStore(QObject* parent=nullptr);
    ~GeneralStore(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // GENERALSTORE_H
