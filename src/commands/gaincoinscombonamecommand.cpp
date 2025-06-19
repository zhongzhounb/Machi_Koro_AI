#include "gaincoinscombonamecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

GainCoinsComboNameCommand::GainCoinsComboNameCommand(Player* sourcePlayer, Card* card,const QString comboName, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::GainCoinsComboName, sourcePlayer,parent,card,nullptr,isFailed,failureMessage),m_comboName(comboName){
}

void GainCoinsComboNameCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    m_cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //没达到前置条件则没有任何效果（目前没有这种效果）
    if(m_isFailed)
        return;
    //计算有多少组合卡
    m_comboNum=m_sourcePlayer->getCardNum(m_comboName,State::None);
    //计算收益
    m_coinsSum=m_cardNum*m_comboNum*m_card->getValue();
    //赚钱
    if(m_coinsSum)
        m_sourcePlayer->addCoins(m_coinsSum);
}

QString GainCoinsComboNameCommand::getLog()const {
    QString log=QString("【%1】%2 %3 ").arg(m_card->getName())
                      .arg(m_cardNum==1?"":QString("*%1").arg(m_cardNum))
                      .arg(m_sourcePlayer->getName());

    if(m_isFailed)//（目前没有这种效果）
        log+=QString("%1。").arg(m_failureMessage);
    else if(m_comboNum)
        log+=QString("没有 %1 ，错过了赚钱的机会。").arg(m_comboName);
    else
        log+=QString("拥有%1个【%2】，获得 %3 金币。").arg(m_comboNum).arg(m_comboName).arg(m_coinsSum);

    return log;
}
