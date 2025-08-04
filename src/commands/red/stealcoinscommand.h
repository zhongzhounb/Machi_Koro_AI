#ifndef STEALCOINSCOMMAND_H
#define STEALCOINSCOMMAND_H
#include "gamecommand.h"

class StealCoinsCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit StealCoinsCommand(Player* player, Card* card,Player* activePlayer=nullptr, QObject* parent = nullptr);
    virtual ~StealCoinsCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;
};

#endif // STEALCOINSCOMMAND_H
