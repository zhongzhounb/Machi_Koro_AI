#ifndef SODABOTTLINGPLANT_H
#define SODABOTTLINGPLANT_H
#include "card.h"

class SodaBottlingPlant: public Card
{
public:
    explicit SodaBottlingPlant(QObject* parent=nullptr);
    ~SodaBottlingPlant(){};
    double getBuyWight(Player* aiPlayer, GameState* gameState) const override;
    QList<GameCommand*> createCommands(Player* owner, Player* activePlayer) override;
    QString getDescription() const override;
private:
    Type m_comboType;
};

#endif // SODABOTTLINGPLANT_H
