#ifndef REROLLDICECOMMAND_H
#define REROLLDICECOMMAND_H
#include "gamecommand.h"

class RerollDiceCommand: public GameCommand
{
public:
    explicit RerollDiceCommand(Player* player, Card* card, QObject* parent = nullptr, bool isAnimation = false, const QString& description = "");
    virtual ~RerollDiceCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_diceNum1;
    int m_diceNum2;
};

#endif // REROLLDICECOMMAND_H
