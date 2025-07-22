#include "initgamecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

InitGameCommand::InitGameCommand( QObject* parent)
    : GameCommand(CommandType::InitGame, nullptr,parent){
}


void InitGameCommand::execute(GameState* state, GameController* controller){
    /*游戏初始设置*/

    //游戏人数设置
    //state->addPlayer(new Player("仲舟",AIRank::None,state));
    //state->addPlayer(new Player("ghost_him",AIRank::None,state));

    //开始初始设置
    state->initState();

};

QString InitGameCommand::getLog() const {
    QString log=QString("欢迎来到骰子街，游戏开始！");
    return log;
};

