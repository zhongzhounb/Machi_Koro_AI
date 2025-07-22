#ifndef GAINCOINSIFNOBUYCARDCOMMAND_H
#define GAINCOINSIFNOBUYCARDCOMMAND_H
#include "gamecommand.h"

class GainCoinsIfNoBuyCardCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit GainCoinsIfNoBuyCardCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~GainCoinsIfNoBuyCardCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_coinsSum; // 存储最终要获得的金币数量
    int m_cardNum;       // 存储触发卡牌的数量 (例如，麦田*3 中的 3)
};

#endif // GAINCOINSIFNOBUYCARDCOMMAND_H
