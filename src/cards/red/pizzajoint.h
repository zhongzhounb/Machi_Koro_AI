#ifndef PIZZAJOINT_H
#define PIZZAJOINT_H
#include "card.h"

class PizzaJoint: public Card
{
    Q_OBJECT
public:
    explicit PizzaJoint(QObject* parent=nullptr);
    ~PizzaJoint(){};
    QString getDescription() const override;
};
#endif // PIZZAJOINT_H
