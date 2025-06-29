#ifndef WHEATFIELD_H
#define WHEATFIELD_H
#include "card.h"

class WheatField: public Card
{
    Q_OBJECT
public:
    explicit WheatField(QObject* parent=nullptr);
    ~WheatField(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // WHEATFIELD_H
