#ifndef RADIOTOWER_H
#define RADIOTOWER_H
#include "card.h"

class RadioTower: public Card
{
    Q_OBJECT
public:
    explicit RadioTower(QObject* parent=nullptr);
    ~RadioTower(){};
    QString getDescription() const override;
};

#endif // RADIOTOWER_H
