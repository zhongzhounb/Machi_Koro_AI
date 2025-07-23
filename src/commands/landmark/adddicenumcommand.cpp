#include "adddicenumcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"

AddDiceNumCommand::AddDiceNumCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::AddDiceNum, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

PromptData AddDiceNumCommand::getPromptData(GameState* state) {
    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择骰子个数阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("你投掷的结果是：%1，你需要让结果+2吗？");
        pt.options.append(OptionData{1,"结果+2",1,""});
        pt.options.append(OptionData{0,"维持原样",1,""});
        return pt;
    }
    }

    return pt;
}
// 获取默认选项（无选项时禁止调用）
int AddDiceNumCommand::getAutoInput( const PromptData& promptData ,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择骰子个数阶段（先默认投一个，做完整个流程会改为投期望最多的一个）
        return 0;
    }
    }
    return 1;

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool AddDiceNumCommand::setInput(int optionId,GameState* state) {
    switch (m_currentStep){
    case 1:{
        m_userInput.append(optionId);
        return true;
    }
    }
    return true;
};

void AddDiceNumCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    m_isAdd=m_userInput[0];

    if(!m_isAdd)
        return;

    Dice* dice=state->getDice();
    dice->setAddNum(2);
}

QString AddDiceNumCommand::getLog()const {
    QString log=QString("【%1】 %2 ").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());

    if(!m_isAdd)
        return "";
    else
        log+=QString("选择将骰子点数+2再执行。");

    return log;
}
