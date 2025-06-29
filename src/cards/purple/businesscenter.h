#ifndef BUSINESSCENTER_H
#define BUSINESSCENTER_H
#include "card.h"

class BusinessCenter: public Card
{
    Q_OBJECT
public:
    explicit BusinessCenter(QObject* parent=nullptr);
    ~BusinessCenter(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // BUSINESSCENTER_H
