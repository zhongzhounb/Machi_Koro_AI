#ifndef MOVINGCOMPANY_H
#define MOVINGCOMPANY_H
#include "card.h"

class MovingCompany: public Card
{
    Q_OBJECT
public:
    explicit MovingCompany(QObject* parent=nullptr);
    ~MovingCompany(){};
    QString getDescription() const override;
};

#endif // MOVINGCOMPANY_H
