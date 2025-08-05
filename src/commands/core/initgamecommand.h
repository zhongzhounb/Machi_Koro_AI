#ifndef INITGAMECOMMAND_H
#define INITGAMECOMMAND_H
#include "gamecommand.h"

class InitGameCommand: public GameCommand
{
    Q_OBJECT
public:
    explicit InitGameCommand(Player* sourcePlayer = nullptr, QObject* parent = nullptr,
                             QList<Card*> cards={},Player* activePlayer=nullptr);
    virtual ~InitGameCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

private:

};

#endif // INITGAMECOMMAND_H
