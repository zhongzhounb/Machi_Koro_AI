#include "rolldicecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"
#include "commandfactory.h"
#include "randomutils.h"
#include "ai/ai.h"

RollDiceCommand::RollDiceCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::RollDice, sourcePlayer,parent,cards,activePlayer){
}

//取期望收益最大的骰子数投掷
PromptData RollDiceCommand::getPromptData(GameState* state) const{
    PromptData pt;
    pt.waitMessage=m_sourcePlayer->getName()+"正在投掷骰子";
    switch (m_currentStep){
    case 1:{//选择骰子个数阶段
        pt.type=PromptData::SelectDice;
        pt.promptMessage=QString("请投掷骰子");
        pt.options.append(OptionData{1,"抛一个",1,""});
        if(m_sourcePlayer->getCardNum("火车站",State::Opening))
            pt.options.append(OptionData{2,"抛两个",1,""});
        else
            pt.options.append(OptionData{2,"抛两个",0,"需要开通火车站"});
        //此处需要更新ai信息
        state->getAI()->update(state);
        //设置默认选项
        pt.autoInput=state->getAI()->getRollDiceNum(m_sourcePlayer);
        return pt;
    }
    case 2:{//骰子滚动动画
        pt.type=PromptData::DiceAnimation;
        pt.diceNum.append(m_diceNum1);
        if(m_diceNum2)
            pt.diceNum.append(m_diceNum2);
        return pt;
    }
    }

    return pt;
}

// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool RollDiceCommand::setInput(int optionId,GameState* state,GameController* controller) {
    switch (m_currentStep){
    case 1:{
        m_userInput.append(optionId);
        //特别的，这里需要提前处理execute的事：获取骰子随机数
        m_diceNum1=RandomUtils::instance().generateInt(1,6);
        if(optionId==2)
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

void RollDiceCommand::execute(GameState* state, GameController* controller){

    //抛骰子
    Dice* dice=state->getDice();
    //保存骰子结果
    dice->setFirstNum(m_diceNum1);
    if(m_diceNum2)
        dice->setSecondNum(m_diceNum2);

    int diceSum=m_diceNum1+m_diceNum2;

    //如果数字大于10且又港口则港口闪烁，但还有重抛，不一定执行
    if(diceSum>=10)
        addCommand("港口",controller);

    //日志
    QString log=QString("%1 投掷了骰子，结果为： ").arg(m_sourcePlayer->getName());
    if(m_diceNum2==0)
        log+=QString("%1。").arg(m_diceNum1);
    else
        log+=QString("%1+%2=%3").arg(m_diceNum1).arg(m_diceNum2).arg(m_diceNum1+m_diceNum2);
    state->addLog(log);

};

