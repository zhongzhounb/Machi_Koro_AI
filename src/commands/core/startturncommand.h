#ifndef STARTTURNCOMMAND_H
#define STARTTURNCOMMAND_H
#include "gamecommand.h"

class StartTurnCommand:public GameCommand
{
    Q_OBJECT
public:
    explicit StartTurnCommand(Player* player, QObject* parent = nullptr);
    virtual ~StartTurnCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
};

#endif // STARTTURNCOMMAND_H
