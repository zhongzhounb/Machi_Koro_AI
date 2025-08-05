#ifndef ACTIVATECARDSCOMMAND_H
#define ACTIVATECARDSCOMMAND_H
#include "gamecommand.h"

class ActivateCardsCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit ActivateCardsCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                                  QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~ActivateCardsCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

};

#endif // ACTIVATECARDSCOMMAND_H
