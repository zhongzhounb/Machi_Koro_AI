#include "gaincoinscombotypecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

GainCoinsComboTypeCommand::GainCoinsComboTypeCommand(Player* sourcePlayer, Card* card,Type comboType,bool isSelf, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::GainCoinsComboType, sourcePlayer,parent,card,nullptr,isFailed,failureMessage),m_comboType(comboType),m_isSelf(isSelf){
}

void GainCoinsComboTypeCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    m_cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //没达到前置条件则没有任何效果（目前没有这种效果）
    if(m_isFailed)
        return;
    //计算有多少组合卡
    if(m_isSelf)//跟自己组合
        m_comboNum=m_sourcePlayer->getTypeCardNum(m_comboType,State::None);
    else{//跟所有组合
        m_comboNum=0;
        for(Player* player:state->getPlayers())
            m_comboNum+=player->getTypeCardNum(m_comboType,State::None);
    }
    //计算收益
    m_coinsSum=m_cardNum*m_comboNum*m_card->getValue();
    //赚钱
    if(m_coinsSum)
        m_sourcePlayer->addCoins(m_coinsSum);
}

QString GainCoinsComboTypeCommand::getLog()const {
    QString log=QString("【%1】%2 %3 ").arg(m_card->getName())
                      .arg(m_cardNum==1?"":QString("*%1").arg(m_cardNum))
                      .arg(m_isSelf?m_sourcePlayer->getName():"场上");

    if(m_isFailed)//（目前没有这种效果）
        log+=QString("%1。").arg(m_failureMessage);
    else if(m_coinsSum==0)
        log+=QString("没有 %1 ，%2错过了赚钱的机会。").arg(typeToImg(m_comboType))
                   .arg(m_isSelf?"":m_sourcePlayer->getName());
    else
        log+=QString("拥有%1个 %2 ，%3获得 %4 金币。").arg(m_comboNum).arg(typeToImg(m_comboType))
                   .arg(m_isSelf?"":m_sourcePlayer->getName()).arg(m_coinsSum);

    return log;
}

