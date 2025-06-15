#ifndef GAINCOINSCOMMAND_H
#define GAINCOINSCOMMAND_H
#include "gamecommand.h"

class GainCoinsCommand: public GameCommand
{
public:
    explicit GainCoinsCommand(Player* player, Card* card, QObject* parent = nullptr, bool isAnimation = false, const QString& description = "");
    virtual ~GainCoinsCommand()= default;
    // 反序列化的无参构造
    GainCoinsCommand();

    void execute(GameState* state, GameController* controller) override;
private:
    int m_coins;
};

#endif // GAINCOINSCOMMAND_H
