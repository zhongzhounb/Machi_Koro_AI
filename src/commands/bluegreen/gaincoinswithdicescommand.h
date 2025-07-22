#ifndef GAINCOINSWITHDICESCOMMAND_H
#define GAINCOINSWITHDICESCOMMAND_H
#include "gamecommand.h"

class GainCoinsWithDicesCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit GainCoinsWithDicesCommand(Player* player, Card* card, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~GainCoinsWithDicesCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_coinsSum; // 存储最终要获得的金币数量
    int m_cardNum;  // 存储触发卡牌的数量 (例如，麦田*3 中的 3)
};

#endif // GAINCOINSWITHDICESCOMMAND_H
