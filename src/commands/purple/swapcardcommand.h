#ifndef SWAPCARDCOMMAND_H
#define SWAPCARDCOMMAND_H
#include "gamecommand.h"

class SwapCardCommand: public GameCommand
{
public:
    explicit SwapCardCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~SwapCardCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    PromptData getPromptData(GameState* state) const override;
    // 获取默认选项（无选项时禁止调用）
    int getAutoInput( const PromptData& promptData ,GameState* state) const override;
    // 设置选项，返回是否要继续获得选项（无选项时禁止调用）
    bool setInput(int optionId,GameState* state) override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    QString m_cardName1; // 自己的卡
    QString m_cardName2; // 别人的卡
    QString m_playerName;//别人的名字
};

#endif // SWAPCARDCOMMAND_H
