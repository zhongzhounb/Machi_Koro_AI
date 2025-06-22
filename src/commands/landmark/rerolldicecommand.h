#ifndef REROLLDICECOMMAND_H
#define REROLLDICECOMMAND_H
#include "gamecommand.h"

class RerollDiceCommand: public GameCommand
{
public:
    explicit RerollDiceCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~RerollDiceCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_diceNum1;
    int m_diceNum2;
};

#endif // REROLLDICECOMMAND_H
