#ifndef FURNITUREFACTORY_H
#define FURNITUREFACTORY_H
#include "card.h"

class FurnitureFactory: public Card
{
    Q_OBJECT
public:
    explicit FurnitureFactory(QObject* parent=nullptr);
    ~FurnitureFactory(){};
    int getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const override;
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
private:
    Type m_comboType;
};

#endif // FURNITUREFACTORY_H
