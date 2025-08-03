#ifndef CAFE_H
#define CAFE_H
#include "card.h"

class Cafe: public Card
{
    Q_OBJECT
public:
    explicit Cafe(QObject* parent=nullptr);
    ~Cafe(){};
    QString getDescription() const override;
};


#endif // CAFE_H
