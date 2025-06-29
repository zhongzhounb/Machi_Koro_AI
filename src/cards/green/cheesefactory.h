#ifndef CHEESEFACTORY_H
#define CHEESEFACTORY_H
#include "card.h"

class CheeseFactory: public Card
{
    Q_OBJECT
public:
    explicit CheeseFactory(QObject* parent=nullptr);
    ~CheeseFactory(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
private:
    Type m_comboType;
};

#endif // CHEESEFACTORY_H
