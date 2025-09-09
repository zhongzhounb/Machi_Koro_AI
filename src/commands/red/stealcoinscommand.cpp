#include "stealcoinscommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

StealCoinsCommand::StealCoinsCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::StealCoins, sourcePlayer,parent,cards,activePlayer){
}

void StealCoinsCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    int cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //计算前置
    int comboNum=m_card->getComboNum(m_sourcePlayer,m_activePlayer,state);
    //计算收益
    int coinsSum=qMin(cardNum*comboNum*m_card->getValue(),m_activePlayer->getCoins());
    //赚钱
    m_sourcePlayer->stealCoins(m_activePlayer,coinsSum);
    //日志
    QString log=QString("【%1】%2 %3 ").arg(m_card->getName())
                      .arg(cardNum==1?"":QString("*%1").arg(cardNum))
                      .arg(m_sourcePlayer->getName());

    if(coinsSum==0){
        if(comboNum==0)
            log+=QString("%1，没偷到 %2 任何钱").arg(m_card->getNoneLog()).arg(m_activePlayer->getName());
        else
            log+=QString("没偷到 %1 任何钱，因为他没钱。").arg(m_activePlayer->getName());
    }
    else
        log+=QString("偷了 %1 的 %2 金币。").arg(m_activePlayer->getName()).arg(coinsSum);

    state->addLog(log);
}

