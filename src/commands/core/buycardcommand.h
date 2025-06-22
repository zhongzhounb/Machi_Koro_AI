#ifndef BUYCARDCOMMAND_H
#define BUYCARDCOMMAND_H
#include "gamecommand.h"

class BuyCardCommand: public GameCommand
{
public:
    explicit BuyCardCommand(Player* player,  QObject* parent = nullptr);
    virtual ~BuyCardCommand()= default;

    // 检查是否需要用户交互（默认不需要交互）
    bool requiresUserInput() const override;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:
    QString m_cardName;
    int m_cardCoins;

};

#endif // BUYCARDCOMMAND_H
