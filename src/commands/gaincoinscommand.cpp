#include "gaincoinscommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

GainCoinsCommand::GainCoinsCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::GainCoins, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

void GainCoinsCommand::execute(GameState* state, GameController* controller) {
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

QString GainCoinsCommand::getLog()const {
    if(m_isFailed)
        return QString("【%1】%2 %3 %4").arg(m_card->getName())
            .arg(m_cardNum==1?"":QString("*%1").arg(m_cardNum))
            .arg(m_sourcePlayer->getName())
            .arg(m_failureMessage);

    return QString("【%1】%2 %3 获得 %4 金币").arg(m_card->getName())
        .arg(m_cardNum==1?"":QString("*%1").arg(m_cardNum))
        .arg(m_sourcePlayer->getName())
        .arg(m_coinsSum);
}

