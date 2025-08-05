#ifndef STEALCOINSCOMMAND_H
#define STEALCOINSCOMMAND_H
#include "gamecommand.h"

class StealCoinsCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit StealCoinsCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                               QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~StealCoinsCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;
};

#endif // STEALCOINSCOMMAND_H
