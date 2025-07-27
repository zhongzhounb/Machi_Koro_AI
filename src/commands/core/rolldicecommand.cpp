#include "rolldicecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"
#include "commandfactory.h"
#include "randomutils.h"
#include "ai/ai.h"
RollDiceCommand::RollDiceCommand(Player* sourcePlayer,  QObject* parent)
    : GameCommand(CommandType::RollDice, sourcePlayer,parent){
}

//取期望收益最大的骰子数投掷
PromptData RollDiceCommand::getPromptData(GameState* state) {
    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择骰子个数阶段
        pt.type=PromptData::SelectDice;
        pt.promptMessage=QString("请投掷骰子");
        pt.options.append(OptionData{1,"抛一个",1,""});
        if(m_sourcePlayer->getCardNum("火车站",State::Opening))
            pt.options.append(OptionData{2,"抛两个",1,""});
        else
            pt.options.append(OptionData{2,"抛两个",0,"需要开通火车站"});
        return pt;
    }
    }

    return pt;
}
// 获取默认选项（无选项时禁止调用）
int RollDiceCommand::getAutoInput( const PromptData& promptData ,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择骰子个数阶段（先默认投一个，做完整个流程会改为投期望最多的一个)
        AI* ai=state->getAI();
        //需要更新下概率
        ai->update(state);
        int opId=ai->getRollDiceNum(m_sourcePlayer);
        return opId;
    }
    }
    return 1;

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool RollDiceCommand::setInput(int optionId,GameState* state) {
    switch (m_currentStep){
    case 1:{
        m_userInput.append(optionId);
        return true;
    }
    }
    return true;
};

void RollDiceCommand::execute(GameState* state, GameController* controller){
    //读取选项
    int diceNum=m_userInput[0];
    //抛骰子
    Dice* dice=state->getDice();
    dice->rollDice(diceNum);
    //保存骰子结果
    m_diceNum1=dice->getFirstNum();
    m_diceNum2=dice->getSecondNum();
    int diceSum=dice->getSum();

    //如果数字大于10且又港口则港口闪烁，但还有重抛，不一定执行
    if(m_sourcePlayer->getCardNum("港口",State::Opening)>0&&diceSum>=10)
        controller->addCommand(m_sourcePlayer->getCardSpecialCommand("港口"));

};

QString RollDiceCommand::getLog() const {
    QString log=QString("%1 投掷了骰子，结果为： ").arg(m_sourcePlayer->getName());

    if(m_diceNum2==0)
        log+=QString("%1。").arg(m_diceNum1);
    else
        log+=QString("%1+%2=%3").arg(m_diceNum1).arg(m_diceNum2).arg(m_diceNum1+m_diceNum2);

    return log;
};
