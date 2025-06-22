#include "gaincoinsifnobuycardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

GainCoinsIfNoBuyCardCommand::GainCoinsIfNoBuyCardCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::GainCoinsIfNoBuyCard, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

void GainCoinsIfNoBuyCardCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    m_cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //没达到前置条件则没有任何效果
    if(m_isFailed)
        return;
    //计算收益
    m_coinsSum=m_cardNum*m_card->getValue();
    //赚钱
    m_sourcePlayer->addCoins(m_coinsSum);
}

QString GainCoinsIfNoBuyCardCommand::getLog()const {
    QString log=QString("【%1】%2 %3 ").arg(m_card->getName())
                      .arg(m_cardNum==1?"":QString("*%1").arg(m_cardNum))
                      .arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else
        log+=QString("因为本轮没建造，获得 %1 金币。").arg(m_coinsSum);

    return log;
}
