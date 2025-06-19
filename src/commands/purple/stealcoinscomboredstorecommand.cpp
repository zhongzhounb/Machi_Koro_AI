#include "stealcoinscomboredstorecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "logutils.h"

StealCoinsComboRedStoreCommand::StealCoinsComboRedStoreCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::StealCoinsComboRedStore, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

void StealCoinsComboRedStoreCommand::execute(GameState* state, GameController* controller) {
    //没达到前置条件则没有任何效果
    if(m_isFailed)
        return;
    //计算收益
    for(Player* player:state->getPlayers()){
        //计算卡
        int cardNum=player->getTypeCardNum(Type::Store,State::None)+player->getTypeCardNum(Type::Restaurant,State::None);
        //计算收益
        int coins=qMin(cardNum*m_card->getValue(),player->getCoins());
        m_coinsToPlayers[coins].append(player->getName());
        m_coinsSum+=coins;
        if(coins>0)
            m_sourcePlayer->stealCoins(player,coins);
    }
}

QString StealCoinsComboRedStoreCommand::getLog()const {
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

