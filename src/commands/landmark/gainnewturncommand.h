#ifndef GAINNEWTURNCOMMAND_H
#define GAINNEWTURNCOMMAND_H
#include "gamecommand.h"

class GainNewTurnCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit GainNewTurnCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~GainNewTurnCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:

};

#endif // GAINNEWTURNCOMMAND_H
