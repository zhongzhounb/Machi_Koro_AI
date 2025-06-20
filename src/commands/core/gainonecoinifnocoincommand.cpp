#include "gainonecoinifnocoincommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

GainOneCoinIfNoCoinCommand::GainOneCoinIfNoCoinCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::GainCoins, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

void GainOneCoinIfNoCoinCommand::execute(GameState* state, GameController* controller) {
    //如果大于0，不执行
    if(m_sourcePlayer->getCoins()>0){
        m_isFailed=true;
        return;
    }

    //赚钱
    m_sourcePlayer->addCoins(1);
}

QString GainOneCoinIfNoCoinCommand::getLog()const {
    QString log=QString(" %1 ").arg(m_sourcePlayer->getName());

    if(m_isFailed)
        return "";
    else
        log+=QString("因为没钱，领了 1 金币的低保。");

    return log;
}
