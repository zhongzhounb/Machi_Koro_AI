#include "adddicenumcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"
#include "ai/ai.h"

AddDiceNumCommand::AddDiceNumCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::AddDiceNum, sourcePlayer,parent,cards,activePlayer){
}

//+2收益高就+2
PromptData AddDiceNumCommand::getPromptData(GameState* state) const{
    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择骰子个数阶段
        pt.type=PromptData::Popup;
        int sum=state->getDice()->getSum();
        pt.promptMessage=QString("你投掷的结果是：%1，你需要让结果+2吗？").arg(sum);
        pt.options.append(OptionData{1,"结果+2",1,""});
        pt.options.append(OptionData{0,"维持原样",1,""});
        //设置默认选项
        pt.autoInput=state->getAI()->getAddDiceNum(m_sourcePlayer,sum);
        return pt;
    }
    }

    return pt;
}

// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool AddDiceNumCommand::setInput(int optionId,GameState* state,GameController* controller) {
    switch (m_currentStep){
    case 1:{
        m_userInput.append(optionId);
        execute(state,controller);
        return true;
    }
    }
    return true;
};

void AddDiceNumCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    int isAdd=m_userInput[0];

    if(!isAdd)
        return;
    //执行
    Dice* dice=state->getDice();
    dice->setAddNum(2);
    //日志
    QString log=QString("【%1】 %2 ").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());
    log+=QString("选择将骰子点数+2再执行。");
    state->addLog(log);
}
