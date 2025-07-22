#include "gainnewturncommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "commandfactory.h"

GainNewTurnCommand::GainNewTurnCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::GainNewTurn, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

void GainNewTurnCommand::execute(GameState* state, GameController* controller) {

    controller->addCommand(CommandFactory::instance().createStartTurnCommand(state->getCurrentPlayer(),this));
}

QString GainNewTurnCommand::getLog()const {
    QString log=QString("【%1】 %2 ").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());

    if(m_isFailed)
        return "";
    else
        log+="因为点数相同获得新的回合！";

    return log;
}
