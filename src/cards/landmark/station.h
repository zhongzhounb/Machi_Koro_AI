#ifndef STATION_H
#define STATION_H
#include "card.h"

class Station: public Card
{
    Q_OBJECT
public:
    explicit Station(QObject* parent=nullptr);
    ~Station(){};
    QString getDescription() const override;
};

#endif // STATION_H
