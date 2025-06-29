#ifndef APPLEORCHARD_H
#define APPLEORCHARD_H
#include "card.h"

class AppleOrchard: public Card
{
    Q_OBJECT
public:
    explicit AppleOrchard(QObject* parent=nullptr);
    ~AppleOrchard(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // APPLEORCHARD_H
