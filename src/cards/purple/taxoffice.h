#ifndef TAXOFFICE_H
#define TAXOFFICE_H
#include "cards/card.h"

class TaxOffice: public Card
{
public:
    explicit TaxOffice(QObject* parent=nullptr);
    ~TaxOffice(){};
    double getBuyWight(Player* aiPlayer, Game* game) const override;
    QString activate(Player* owner, Player* activePlayer,Game* game ,const QVariant& choiceData) override;
};

#endif // TAXOFFICE_H
