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
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
private:
    Type m_comboType;
};

#endif // SODABOTTLINGPLANT_H
