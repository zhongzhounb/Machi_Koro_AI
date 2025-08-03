#ifndef AMUSEMENTPARK_H
#define AMUSEMENTPARK_H
#include "card.h"

class AmusementPark: public Card
{
    Q_OBJECT
public:
    explicit AmusementPark(QObject* parent=nullptr);
    ~AmusementPark(){};
    QString getDescription() const override;
};

#endif // AMUSEMENTPARK_H
