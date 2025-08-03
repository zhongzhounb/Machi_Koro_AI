#ifndef HARBOR_H
#define HARBOR_H
#include "card.h"

class Harbor: public Card
{
    Q_OBJECT
public:
    explicit Harbor(QObject* parent=nullptr);
    ~Harbor(){};
    QString getDescription() const override;
};

#endif // HARBOR_H
