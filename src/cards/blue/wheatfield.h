#ifndef WHEATFIELD_H
#define WHEATFIELD_H
#include "card.h"

class WheatField: public Card
{
    Q_OBJECT
public:
    explicit WheatField(QObject* parent=nullptr);
    ~WheatField(){};
    QString getDescription() const override;
};

#endif // WHEATFIELD_H
