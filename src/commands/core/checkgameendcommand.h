#ifndef CHECKGAMEENDCOMMAND_H
#define CHECKGAMEENDCOMMAND_H

#include "gamecommand.h"

class CheckGameEndCommand : public GameCommand
{
    Q_OBJECT
public:
    explicit CheckGameEndCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                                 QList<Card*> cards = {}, Player* activePlayer = nullptr);
    virtual ~CheckGameEndCommand() = default;

    PromptData getPromptData(GameState* state) const override;
    bool setInput(int optionId, GameState* state, GameController* controller = nullptr) override;
    void execute(GameState* state, GameController* controller = nullptr) override;

private:
    bool isWinCondition(Player* player) const;
    mutable bool m_isWin = false;
};

#endif // CHECKGAMEENDCOMMAND_H
