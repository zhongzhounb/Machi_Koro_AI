#include "rerolldicecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"
#include "commandfactory.h"

RerollDiceCommand::RerollDiceCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::RerollDice, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

PromptData RerollDiceCommand::getPromptData(GameState* state) {
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
        return pt;
    }
    }

    return pt;
}
// 获取默认选项（无选项时禁止调用）
int RerollDiceCommand::getAutoInput( const PromptData& promptData ,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择骰子个数阶段（先默认投一个，做完整个流程会改为投期望最多的一个）
        return 0;
    }
    }
    return 1;

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool RerollDiceCommand::setInput(int optionId,GameState* state) {
    switch (m_currentStep){
    case 1:{
        m_userInput.append(optionId);
        return true;
    }
    }
    return true;
};

void RerollDiceCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    int diceNum=m_userInput[0];
    //如果不抛
    if(diceNum==0){
        m_isFailed=true;
        return;
    }
    //抛骰子
    Dice* dice=state->getDice();
    dice->rollDice(diceNum);
    //保存骰子结果
    m_diceNum1=dice->getFirstNum();
    m_diceNum2=dice->getSecondNum();
    int diceSum=dice->getSum();

    //创建命令
    if(m_sourcePlayer->getCardNum("港口",State::Opening)>0){
        QList<GameCommand*> commands=controller->getCommands(CommandType::AddDiceNum);
        //新骰子符合且没有+2则添加
        if(diceSum>=10&&commands.size()==0)
            controller->addCommand(m_sourcePlayer->getCardSpecialCommand("港口"));
        //新骰子不符合且有+2则删除
        else if(diceSum<10&&commands.size()!=0)
            controller->delCommand(commands.at(0));
            //其他情况不变
    }

}

QString RerollDiceCommand::getLog()const {
    QString log=QString("【%1】 %2 ").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());

    if(m_isFailed)
        return "";
    else if(m_diceNum2==0)
        log+=QString("%1。").arg(m_diceNum1);
    else
        log+=QString("%1+%2=%3").arg(m_diceNum1).arg(m_diceNum2).arg(m_diceNum1+m_diceNum2);

    return log;
}


