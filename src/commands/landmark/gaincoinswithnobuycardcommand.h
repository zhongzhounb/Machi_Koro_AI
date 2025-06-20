#ifndef GAINCOINSWITHNOBUYCARDCOMMAND_H
#define GAINCOINSWITHNOBUYCARDCOMMAND_H
#include "gamecommand.h"

class GainCoinsWithNoBuyCardCommand: public GameCommand
{
public:
    explicit GainCoinsWithNoBuyCardCommand(Player* player, Card* card, QObject* parent = nullptr, bool isAnimation = false, const QString& description = "");
    virtual ~GainCoinsWithNoBuyCardCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    int m_coinsSum; // 存储最终要获得的金币数量
    int m_cardNum;       // 存储触发卡牌的数量 (例如，麦田*3 中的 3)
};

#endif // GAINCOINSWITHNOBUYCARDCOMMAND_H
