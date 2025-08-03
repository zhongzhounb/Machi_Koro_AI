#ifndef TECHSTARTUP_H
#define TECHSTARTUP_H
#include "card.h"

class TechStartup: public Card
{
    Q_OBJECT
public:
    explicit TechStartup(QObject* parent=nullptr);
    ~TechStartup(){};
    QString getDescription() const override;
};

#endif // TECHSTARTUP_H
