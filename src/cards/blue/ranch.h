#ifndef RANCH_H
#define RANCH_H
#include "card.h"

class Ranch: public Card
{
    Q_OBJECT
public:
    explicit Ranch(QObject* parent=nullptr);
    ~Ranch(){};
    QString getDescription() const override;
};

#endif // RANCH_H
