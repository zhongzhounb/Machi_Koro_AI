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

    controller->addCommand(CommandFactory::instance().createStartTurnCommand(state->getCurrentPlayer(),this));
    QString log=QString("【%1】 %2 因为点数相同获得新的回合！").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());

    state->addLog(log);
}

