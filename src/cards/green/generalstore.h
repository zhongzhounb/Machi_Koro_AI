#ifndef GENERALSTORE_H
#define GENERALSTORE_H
#include "card.h"

class GeneralStore: public Card
{
    Q_OBJECT
public:
    explicit GeneralStore(QObject* parent=nullptr);
    ~GeneralStore(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    QString getDescription() const override;
};

#endif // GENERALSTORE_H
