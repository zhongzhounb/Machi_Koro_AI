#ifndef SODABOTTLINGPLANT_H
#define SODABOTTLINGPLANT_H
#include "card.h"

class SodaBottlingPlant: public Card
{
    Q_OBJECT
public:
    explicit SodaBottlingPlant(QObject* parent=nullptr);
    ~SodaBottlingPlant(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    QString getDescription() const override;
};

#endif // SODABOTTLINGPLANT_H
