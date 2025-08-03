#ifndef FAMILYRESTAURANT_H
#define FAMILYRESTAURANT_H
#include "card.h"

class FamilyRestaurant: public Card
{
    Q_OBJECT
public:
    explicit FamilyRestaurant(QObject* parent=nullptr);
    ~FamilyRestaurant(){};
    QString getDescription() const override;
};

#endif // FAMILYRESTAURANT_H
