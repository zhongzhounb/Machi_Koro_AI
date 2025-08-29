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
    case 1:{//选择骰子个数阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("请抛两个骰子来决定收益");
        pt.options.append(OptionData{1,"抛两个",1,""});
        //设置默认选项（此处没得选）
        pt.autoInput=1;
        return pt;
    }
    case 2:{//骰子滚动动画
        pt.type=PromptData::DiceAnimation;
        pt.diceNum.append(m_diceNum1);
        pt.diceNum.append(m_diceNum2);
        return pt;
    }
    }
    return pt;
}

// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool GainCoinsWithDicesCommand::setInput(int optionId,GameState* state,GameController* controller) {
    switch (m_currentStep){
    case 1:{
        m_userInput.append(optionId);

        m_diceNum1=RandomUtils::instance().generateInt(1,6);
        m_diceNum2=RandomUtils::instance().generateInt(1,6);

        m_currentStep=2;
        return false;
    }
    case 2:{
        execute(state,controller);
        return true;
    }
    }
    return true;
};

void GainCoinsWithDicesCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    int cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //计算有多少组合
    int comboNum=m_card->getComboNum(m_sourcePlayer,m_sourcePlayer,state);
    //计算收益
    int coinsSum=cardNum*comboNum*(m_diceNum1+m_diceNum2);
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

