#ifndef APPLEORCHARD_H
#define APPLEORCHARD_H
#include "card.h"

class AppleOrchard: public Card
{
    Q_OBJECT
public:
    explicit AppleOrchard(QObject* parent=nullptr);
    ~AppleOrchard(){};
    QString getDescription() const override;
};

#endif // APPLEORCHARD_H
