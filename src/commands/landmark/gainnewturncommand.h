#ifndef GAINNEWTURNCOMMAND_H
#define GAINNEWTURNCOMMAND_H
#include "gamecommand.h"

class GainNewTurnCommand: public GameCommand
{
public:
    explicit GainNewTurnCommand(Player* player, Card* card, QObject* parent = nullptr, bool isAnimation = false, const QString& description = "");
    virtual ~GainNewTurnCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:

};

#endif // GAINNEWTURNCOMMAND_H
