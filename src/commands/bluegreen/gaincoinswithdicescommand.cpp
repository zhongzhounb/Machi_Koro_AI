#include "gaincoinswithdicescommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"
#include "randomutils.h"

GainCoinsWithDicesCommand::GainCoinsWithDicesCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::GainCoinsWithDices, sourcePlayer,parent,cards,activePlayer){
}

//如果小于期望收益就重抛
PromptData GainCoinsWithDicesCommand::getPromptData(GameState* state) const{
    PromptData pt;
    switch (m_currentStep){
    case 1:{//骰子滚动动画（因为会有多个命令，所以只由第一个命令执行）
        Dice* dice=state->getDiceTemp();
        if(dice->getSum()==0){
            //抛两个骰子
            dice->rollDice(2);
            pt.type=PromptData::DiceAnimation;
            pt.diceNum.append(dice->getFirstNum());
            pt.diceNum.append(dice->getSecondNum());
        }
        return pt;
    }
    }
    return pt;
}

void GainCoinsWithDicesCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    int cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //计算有多少组合
    int comboNum=m_card->getComboNum(m_sourcePlayer,m_sourcePlayer,state);
    //计算收益
    Dice* dice=state->getDiceTemp();
    int coinsSum=cardNum*comboNum*(dice->getSum());
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

