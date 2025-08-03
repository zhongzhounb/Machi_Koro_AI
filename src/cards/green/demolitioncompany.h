#ifndef DEMOLITIONCOMPANY_H
#define DEMOLITIONCOMPANY_H
#include "card.h"

class DemolitionCompany: public Card
{
    Q_OBJECT
public:
    explicit DemolitionCompany(QObject* parent=nullptr);
    ~DemolitionCompany(){};
    QString getDescription() const override;
};

#endif // DEMOLITIONCOMPANY_H
