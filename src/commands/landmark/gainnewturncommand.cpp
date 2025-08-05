#include "gainnewturncommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "commandfactory.h"

GainNewTurnCommand::GainNewTurnCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::GainNewTurn, sourcePlayer,parent,cards,activePlayer){
}

void GainNewTurnCommand::execute(GameState* state, GameController* controller) {
    //递归添加回合命令
    controller->addCommand(CommandFactory::instance().createCommand(StartTurn,m_sourcePlayer,controller));

    QString log=QString("【%1】 %2 因为点数相同获得新的回合！").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());

    state->addLog(log);
}

