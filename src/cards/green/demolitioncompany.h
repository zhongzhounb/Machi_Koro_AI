#ifndef DEMOLITIONCOMPANY_H
#define DEMOLITIONCOMPANY_H
#include "card.h"

class DemolitionCompany: public Card
{
    Q_OBJECT
public:
    explicit DemolitionCompany(QObject* parent=nullptr);
    ~DemolitionCompany(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // DEMOLITIONCOMPANY_H
