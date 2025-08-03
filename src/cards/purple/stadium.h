#ifndef STADIUM_H
#define STADIUM_H
#include "card.h"

class Stadium: public Card
{
    Q_OBJECT
public:
    explicit Stadium(QObject* parent=nullptr);
    ~Stadium(){};
    QString getDescription() const override;
};

#endif // STADIUM_H
