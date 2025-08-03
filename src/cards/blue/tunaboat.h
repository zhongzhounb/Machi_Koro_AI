#ifndef TUNABOAT_H
#define TUNABOAT_H
#include "card.h"

class TunaBoat: public Card
{
    Q_OBJECT
public:
    explicit TunaBoat(QObject* parent=nullptr);
    ~TunaBoat(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    QString getDescription() const override;
};

#endif // TUNABOAT_H
