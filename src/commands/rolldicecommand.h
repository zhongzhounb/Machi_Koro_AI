#ifndef ROLLDICECOMMAND_H
#define ROLLDICECOMMAND_H
#include "gamecommand.h"

class RollDiceCommand: public GameCommand
{
public:
    explicit RollDiceCommand(Player* player, QObject* parent = nullptr);
    virtual ~RollDiceCommand()= default;

    void execute(GameState* state, GameController* controller) override;

    QString getLog() const override;
};

#endif // ROLLDICECOMMAND_H
