#ifndef GAINONECOINIFNOCOINSCOMMAND_H
#define GAINONECOINIFNOCOINSCOMMAND_H
#include "gamecommand.h"

class GainOneCoinIfNoCoinsCommand: public GameCommand
{
public:
    explicit GainOneCoinIfNoCoinsCommand(Player* player,  QObject* parent = nullptr);
    virtual ~GainOneCoinIfNoCoinsCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
};

#endif // GAINONECOINIFNOCOINSCOMMAND_H
