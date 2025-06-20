#ifndef GAINONECOINIFNOCOINCOMMAND_H
#define GAINONECOINIFNOCOINCOMMAND_H
#include "gamecommand.h"

class GainOneCoinIfNoCoinCommand: public GameCommand
{
public:
    explicit GainOneCoinIfNoCoinCommand(Player* player, Card* card, QObject* parent = nullptr, bool isAnimation = false, const QString& description = "");
    virtual ~GainOneCoinIfNoCoinCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
};

#endif // GAINONECOINIFNOCOINCOMMAND_H
