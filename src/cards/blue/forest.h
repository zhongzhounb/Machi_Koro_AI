#ifndef FOREST_H
#define FOREST_H
#include "card.h"

class Forest: public Card
{
    Q_OBJECT
public:
    explicit Forest(QObject* parent=nullptr);
    ~Forest(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // FOREST_H
