#ifndef BUSINESSCENTER_H
#define BUSINESSCENTER_H
#include "card.h"

class BusinessCenter: public Card
{
    Q_OBJECT
public:
    explicit BusinessCenter(QObject* parent=nullptr);
    ~BusinessCenter(){};
    QString getDescription() const override;
};

#endif // BUSINESSCENTER_H
