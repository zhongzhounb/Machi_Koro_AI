#ifndef FLOWERORCHARD_H
#define FLOWERORCHARD_H
#include "card.h"

class FlowerOrchard: public Card
{
    Q_OBJECT
public:
    explicit FlowerOrchard(QObject* parent=nullptr);
    ~FlowerOrchard(){};
    QString getDescription() const override;
};
#endif // FLOWERORCHARD_H
