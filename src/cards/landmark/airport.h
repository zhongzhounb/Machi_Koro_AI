#ifndef AIRPORT_H
#define AIRPORT_H
#include "card.h"

class Airport: public Card
{
    Q_OBJECT
public:
    explicit Airport(QObject* parent=nullptr);
    ~Airport(){};
    QString getDescription() const override;
};

#endif // AIRPORT_H
