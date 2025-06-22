#ifndef GIVECARDCOMMAND_H
#define GIVECARDCOMMAND_H
#include "gamecommand.h"

class GiveCardCommand: public GameCommand
{
public:
    explicit GiveCardCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~GiveCardCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_coinsSum; // 存储最终要获得的金币数量
    QString m_cardName;//获得的卡name
    QString m_playerName;//赠予玩家name
};

#endif // GIVECARDCOMMAND_H
