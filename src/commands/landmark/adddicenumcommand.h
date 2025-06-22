#ifndef ADDDICENUMCOMMAND_H
#define ADDDICENUMCOMMAND_H
#include "gamecommand.h"

class AddDiceNumCommand: public GameCommand
{
public:
    explicit AddDiceNumCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~AddDiceNumCommand()= default;
    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    bool m_isAdd;
};

#endif // ADDDICENUMCOMMAND_H
