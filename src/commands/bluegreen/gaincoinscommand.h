#ifndef GAINCOINSCOMMAND_H
#define GAINCOINSCOMMAND_H
#include "gamecommand.h"

class GainCoinsCommand: public GameCommand
{
public:
    explicit GainCoinsCommand(Player* player, Card* card, QObject* parent = nullptr, bool isAnimation = false, const QString& description = "");
    virtual ~GainCoinsCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_coinsSum; // 存储最终要获得的金币数量
    int m_cardNum;       // 存储触发卡牌的数量 (例如，麦田*3 中的 3)
};

#endif // GAINCOINSCOMMAND_H
