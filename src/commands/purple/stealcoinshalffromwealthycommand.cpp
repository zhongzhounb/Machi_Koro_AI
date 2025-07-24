#include "stealcoinshalffromwealthycommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "logutils.h"

StealCoinsHalfFromWealthyCommand::StealCoinsHalfFromWealthyCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::StealCoinsHalfFromWealthy, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

void StealCoinsHalfFromWealthyCommand::execute(GameState* state, GameController* controller) {
    //没达到前置条件则没有任何效果（暂无）
    if(m_isFailed)
        return;
    //计算收益
    for(Player* player:state->getPlayers())
        if(player!=m_sourcePlayer){
            int coins=player->getCoins();
            if(coins>=10)
                coins/=2;
            else
                coins=0;
            m_coinsToPlayers[coins].append(player->getName());
            m_coinsSum+=coins;
            //赚钱
            if(coins>0)
                m_sourcePlayer->stealCoins(player,coins);
        }
}

QString StealCoinsHalfFromWealthyCommand::getLog()const {
    QString log=QString("【%1】 %2 ").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else if(m_coinsSum==0)
        log+="毛都没偷到！";
    else
        log+=LogUtils::formatMutiStealCoinsLog(m_coinsToPlayers)+QString("，共偷到 %1 金币。").arg(m_coinsSum);
    return log;
}
