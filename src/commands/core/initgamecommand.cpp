#include "initgamecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"


InitGameCommand::InitGameCommand( QObject* parent)
    : GameCommand(CommandType::RollDice, nullptr,parent){
}


void InitGameCommand::execute(GameState* state, GameController* controller){
    /*游戏初始设置*/

    //游戏人数设置


};

QString InitGameCommand::getLog() const {
    QString log=QString("欢迎来到骰子街，游戏开始！").arg(m_sourcePlayer->getName());
    return log;
};

