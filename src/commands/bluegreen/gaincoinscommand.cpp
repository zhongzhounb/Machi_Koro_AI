#include "gaincoinscommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

GainCoinsCommand::GainCoinsCommand(Player* sourcePlayer, Card* card, QObject* parent)
    : GameCommand(CommandType::GainCoins, sourcePlayer,parent,card,nullptr){}

void GainCoinsCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    int cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //计算有多少组合
    int comboNum=m_card->getComboNum(m_sourcePlayer,m_sourcePlayer,state);
    //计算收益
    int coinsSum=cardNum*comboNum*m_card->getValue();
    //赚钱
    m_sourcePlayer->addCoins(coinsSum);
    //日志
    QString log=QString("【%1】%2").arg(m_card->getName())
                      .arg(cardNum==1?"":QString("*%1").arg(cardNum));

    if(m_card->getComboLog()!="")
        log+=m_card->getComboLog().arg(comboNum);

    log+=m_sourcePlayer->getName();

    if(m_card->getNoneLog()!=""&&coinsSum==0)
        log+=m_card->getNoneLog();

    if(coinsSum==0)
        log+=QString("没有收益。");
    else
        log+=QString("获得 %1 金币。").arg(coinsSum);

    state->addLog(log);
}



