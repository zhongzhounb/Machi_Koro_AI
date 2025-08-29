#include "rerolldicecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"
#include "ai/ai.h"
#include "randomutils.h"

RerollDiceCommand::RerollDiceCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::RerollDice, sourcePlayer,parent,cards,activePlayer){
}

//如果小于期望收益就重抛
PromptData RerollDiceCommand::getPromptData(GameState* state) const{
    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择骰子个数阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("你投掷的结果是：%1，你需要重抛吗？");
        pt.options.append(OptionData{1,"抛一个",1,""});
        if(m_sourcePlayer->getCardNum("火车站",State::Opening))
            pt.options.append(OptionData{2,"抛两个",1,""});
        else
            pt.options.append(OptionData{2,"抛两个",0,"需要开通火车站"});
        pt.options.append(OptionData{0,"维持原样",1});
        //设置默认选项
        pt.autoInput=state->getAI()->getReRollDiceNum(m_sourcePlayer,state->getDice()->getFirstNum(),state->getDice()->getSecondNum());
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
bool RerollDiceCommand::setInput(int optionId,GameState* state,GameController* controller) {
    switch (m_currentStep){
    case 1:{
        m_userInput.append(optionId);

        //如果维持原样
        if(optionId==0)
            return true;

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

void RerollDiceCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    int diceNum=m_userInput[0];
    //如果不抛
    if(diceNum==0)
        return;

    //抛骰子
    Dice* dice=state->getDice();
    //保存骰子结果
    dice->setFirstNum(m_diceNum1);
    if(m_diceNum2)
        dice->setSecondNum(m_diceNum2);
    int diceSum=dice->getSum();

    //创建命令
    QList<GameCommand*> commands=controller->getCommands(CommandType::AddDiceNum);
    //新骰子符合且没有+2则添加
    if(diceSum>=10&&commands.size()==0)
        addCommand("港口",controller);
    //新骰子不符合且有+2则删除
    else if(diceSum<10&&commands.size()!=0)
        delCommand("港口",controller);

    //日志
    QString log=QString("【%1】 %2 重新投掷了骰子，结果为：").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());
    if(m_diceNum2==0)
        log+=QString("%1。").arg(m_diceNum1);
    else
        log+=QString("%1+%2=%3").arg(m_diceNum1).arg(m_diceNum2).arg(m_diceNum1+m_diceNum2);
    state->addLog(log);

}


