#ifndef STEALCOINSCOMMAND_H
#define STEALCOINSCOMMAND_H
#include "gamecommand.h"

class StealCoinsCommand: public GameCommand
{
public:
    explicit StealCoinsCommand(Player* player, Card* card,Player* activePlayer=nullptr, QObject* parent = nullptr, bool isFailed = false, const QString& failureMessage = "");
    virtual ~StealCoinsCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_coinsSum; // 存储最终要获得的金币数量
    int m_cardNum;  // 存储触发卡牌的数量 (例如，麦田*3 中的 3)
};

#endif // STEALCOINSCOMMAND_H
