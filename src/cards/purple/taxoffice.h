#ifndef TAXOFFICE_H
#define TAXOFFICE_H
#include "card.h"

class TaxOffice: public Card
{
    Q_OBJECT
public:
    explicit TaxOffice(QObject* parent=nullptr);
    ~TaxOffice(){};
    int getStealCoins(Player* player)const override;
    QString getDescription() const override;
};

#endif // TAXOFFICE_H
