#ifndef FOREST_H
#define FOREST_H
#include "card.h"

class Forest: public Card
{
    Q_OBJECT
public:
    explicit Forest(QObject* parent=nullptr);
    ~Forest(){};
    QString getDescription() const override;
};

#endif // FOREST_H
