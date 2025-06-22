#ifndef ROLLDICECOMMAND_H
#define ROLLDICECOMMAND_H
#include "gamecommand.h"

class RollDiceCommand: public GameCommand
{
public:
    explicit RollDiceCommand(Player* sourcePlayer,  QObject* parent=nullptr);
    virtual ~RollDiceCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;

private:
    int m_diceNum1;
    int m_diceNum2;
};

#endif // ROLLDICECOMMAND_H
