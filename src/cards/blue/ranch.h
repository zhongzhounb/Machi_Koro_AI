#ifndef RANCH_H
#define RANCH_H
#include "card.h"

class Ranch: public Card
{
    Q_OBJECT
public:
    explicit Ranch(QObject* parent=nullptr);
    ~Ranch(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // RANCH_H
