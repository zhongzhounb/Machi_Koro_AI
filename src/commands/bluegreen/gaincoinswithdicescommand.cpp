#include "gaincoinswithdicescommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"

GainCoinsWithDicesCommand::GainCoinsWithDicesCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::GainCoinsWithDices, sourcePlayer,parent,cards,activePlayer){
}

void GainCoinsWithDicesCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    m_cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //没达到前置条件则没有任何效果
    if(m_isFailed)
        return;
    //抛骰子（临时骰子）
    Dice* dice=state->getTempDice();
    if(dice->getSum()==0)
        dice->rollDice(2);
    //计算收益
    m_coinsSum=m_cardNum*dice->getSum();
    //赚钱
    m_sourcePlayer->addCoins(m_coinsSum);
}

QString GainCoinsWithDicesCommand::getLog()const {
    QString log=QString("【%1】%2 %3 ").arg(m_card->getName())
                      .arg(m_cardNum==1?"":QString("*%1").arg(m_cardNum))
                      .arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else
        log+=QString("获得 %1 金币。").arg(m_coinsSum);

    return log;
}
