#include "investtechnologycompanycommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include"ai/ai.h"
InvestTechnologyCompanyCommand::InvestTechnologyCompanyCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::InvestTechnologyCompany, sourcePlayer,parent,cards,activePlayer){
}


PromptData InvestTechnologyCompanyCommand::getPromptData(GameState* state) const{
    //如果没有钱，直接不触发交互
    if(m_sourcePlayer->getCoins()<1)
        return {PromptData::None, "", {}};

    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择是否投资阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("你的【科技公司】已经投资了：%1 金币，你可以再为它投资1金币。").arg(m_card->getValue());
        pt.options.append(OptionData{1,"投资1金币",1,""});
        pt.options.append(OptionData{0,"不投资",1,""});
        //设置默认选项
        pt.autoInput=state->getAI()->getInvestId(pt,m_sourcePlayer,state);
        return pt;
    }
    }

    return pt;
}

// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool InvestTechnologyCompanyCommand::setInput(int optionId,GameState* state,GameController* controller) {
    switch (m_currentStep){
    case 1:{//选择是否投资阶段
        m_userInput.append(optionId);

        execute(state,controller);
        return true;
    }
    }
    return true;
};

void InvestTechnologyCompanyCommand::execute(GameState* state, GameController* controller) {
    QString log=QString("【%1】 %2 ").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());

    if(m_sourcePlayer->getCoins()<1){
        log+="没钱投资了。";
        state->addLog(log);
        return ;
    }

    //读取选项
    int value=m_userInput[0];

    //不投资，不产生日志
    if(value==0){
        log+="选择不投资。";
        state->addLog(log);
        return;
    }

    //投资
    m_sourcePlayer->delCoins(1);
    m_card->changeValue(1);
    log+=QString("投资了 1 金币 ，目前共投资了 %1 金币！").arg(m_card->getValue());
    state->addLog(log);
}

