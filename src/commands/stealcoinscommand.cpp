#include "stealcoinscommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

StealCoinsCommand::StealCoinsCommand(Player* sourcePlayer, Card* card,Player* activePlayer, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::StealCoins, sourcePlayer,parent,card,activePlayer,isFailed,failureMessage){
}

void StealCoinsCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    m_cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //没达到前置条件则没有任何效果
    if(m_isFailed)
        return;
    //计算收益
    m_coinsSum=qMin(m_cardNum*m_card->getValue(),m_activePlayer->getCoins());
    //赚钱
    if(m_coinsSum>0)
        m_sourcePlayer->stealCoins(m_activePlayer,m_coinsSum);
}

QString StealCoinsCommand::getLog()const {
    QString log=QString("【%1】%2 %3 ").arg(m_card->getName())
                      .arg(m_cardNum==1?"":QString("*%1").arg(m_cardNum))
                      .arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else if(m_coinsSum==0)
        log+=QString("没偷到 %1 任何钱，因为他没钱。").arg(m_activePlayer->getName());
    else
        log+=QString("偷了 %1 的 %2 金币。").arg(m_activePlayer->getName()).arg(m_coinsSum);

    return log;
}
