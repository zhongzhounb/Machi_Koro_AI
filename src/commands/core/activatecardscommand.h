#ifndef ACTIVATECARDSCOMMAND_H
#define ACTIVATECARDSCOMMAND_H
#include "gamecommand.h"

class ActivateCardsCommand: public GameCommand
{
public:
    explicit ActivateCardsCommand(Player* player,  QObject* parent = nullptr);
    virtual ~ActivateCardsCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_rollSum;
};

#endif // ACTIVATECARDSCOMMAND_H
