#ifndef STADIUM_H
#define STADIUM_H
#include "card.h"

class Stadium: public Card
{
    Q_OBJECT
public:
    explicit Stadium(QObject* parent=nullptr);
    ~Stadium(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // STADIUM_H
