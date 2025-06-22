#ifndef INITGAMECOMMAND_H
#define INITGAMECOMMAND_H
#include "gamecommand.h"

class InitGameCommand: public GameCommand
{
public:
    explicit InitGameCommand(QObject* parent = nullptr);
    virtual ~InitGameCommand()= default;

    void execute(GameState* state, GameController* controller=nullptr) override;

    QString getLog() const override;
private:

};

#endif // INITGAMECOMMAND_H
