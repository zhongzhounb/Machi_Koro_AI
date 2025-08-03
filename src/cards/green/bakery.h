#ifndef BAKERY_H
#define BAKERY_H
#include "card.h"

class Bakery: public Card
{
    Q_OBJECT
public:
    explicit Bakery(QObject* parent=nullptr);
    ~Bakery(){};
    QString getDescription() const override;
};

#endif // BAKERY_H
