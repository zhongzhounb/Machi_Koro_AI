#ifndef GAINNEWTURNCOMMAND_H
#define GAINNEWTURNCOMMAND_H
#include "gamecommand.h"

class GainNewTurnCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit GainNewTurnCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                                QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~GainNewTurnCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;
};

#endif // GAINNEWTURNCOMMAND_H
