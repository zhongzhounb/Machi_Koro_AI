#ifndef REROLLDICECOMMAND_H
#define REROLLDICECOMMAND_H
#include "gamecommand.h"

class RerollDiceCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit RerollDiceCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~RerollDiceCommand()= default;
    // 检查是否需要用户交互（默认不需要交互）
    PromptData getPromptData(GameState* state) override;
    // 获取默认选项（无选项时禁止调用）
    int getAutoInput( const PromptData& promptData ,GameState* state) override;
    // 设置选项，返回是否要继续获得选项（无选项时禁止调用）
    bool setInput(int optionId,GameState* state) override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_diceNum1;
    int m_diceNum2;
};

#endif // REROLLDICECOMMAND_H
