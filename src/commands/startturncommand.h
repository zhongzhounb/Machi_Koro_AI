#ifndef STARTTURNCOMMAND_H
#define STARTTURNCOMMAND_H
#include"global.h"
#include"gamecommand.h"
class Player;
class GameState;
class GameController;

class StartTurnCommand:public GameCommand
{
public:
    explicit StartTurnCommand(Player* player, QObject* parent = nullptr);
    virtual ~StartTurnCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
};

#endif // STARTTURNCOMMAND_H
