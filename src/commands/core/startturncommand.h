#ifndef STARTTURNCOMMAND_H
#define STARTTURNCOMMAND_H
#include "gamecommand.h"

class StartTurnCommand:public GameCommand
{
    Q_OBJECT
public:
    explicit StartTurnCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                              QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~StartTurnCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

};

#endif // STARTTURNCOMMAND_H
