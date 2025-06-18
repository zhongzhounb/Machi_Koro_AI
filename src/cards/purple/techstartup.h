#ifndef TECHSTARTUP_H
#define TECHSTARTUP_H
#include "card.h"

class TechStartup: public Card
{
public:
    explicit TechStartup(QObject* parent=nullptr);
    ~TechStartup(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // TECHSTARTUP_H
