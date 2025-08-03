#ifndef PUBLISHER_H
#define PUBLISHER_H
#include "card.h"

class Publisher: public Card
{
    Q_OBJECT
public:
    explicit Publisher(QObject* parent=nullptr);
    ~Publisher(){};
    int getStealCoins(Player* player)const override;
    QString getDescription() const override;
};

#endif // PUBLISHER_H
