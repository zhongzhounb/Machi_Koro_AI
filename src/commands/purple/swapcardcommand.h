#ifndef SWAPCARDCOMMAND_H
#define SWAPCARDCOMMAND_H
#include "gamecommand.h"

class SwapCardCommand: public GameCommand
{
public:
    explicit SwapCardCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~SwapCardCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    QString m_cardName1; // 自己的卡
    QString m_cardName2; // 别人的卡
    QString m_playerName;//别人的名字
};

#endif // SWAPCARDCOMMAND_H
