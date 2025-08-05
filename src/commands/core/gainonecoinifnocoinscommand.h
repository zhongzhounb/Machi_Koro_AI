#ifndef GAINONECOINIFNOCOINSCOMMAND_H
#define GAINONECOINIFNOCOINSCOMMAND_H
#include "gamecommand.h"

class GainOneCoinIfNoCoinsCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit GainOneCoinIfNoCoinsCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                                         QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~GainOneCoinIfNoCoinsCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

};

#endif // GAINONECOINIFNOCOINSCOMMAND_H
