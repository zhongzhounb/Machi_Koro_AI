#ifndef TAXOFFICE_H
#define TAXOFFICE_H
#include "card.h"

class TaxOffice: public Card
{
public:
    explicit TaxOffice(QObject* parent=nullptr);
    ~TaxOffice(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
};

#endif // TAXOFFICE_H
