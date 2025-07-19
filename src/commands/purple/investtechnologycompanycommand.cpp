#include "investtechnologycompanycommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

InvestTechnologyCompanyCommand::InvestTechnologyCompanyCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::InvestTechnologyCompany, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}


PromptData InvestTechnologyCompanyCommand::getPromptData(GameState* state) {
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

        return pt;
    }
    }

    return pt;
}
// 获取默认选项（无选项时禁止调用）
int InvestTechnologyCompanyCommand::getAutoInput( const PromptData& promptData ,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择是否投资阶段
        if(m_card->getValue()<10)//如果投资的钱小于10元就投资，否则不投资
            return 1;
        return 0;
    }
    }
    return 1;

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool InvestTechnologyCompanyCommand::setInput(int optionId,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择是否投资阶段
        m_userInput.append(optionId);
        return true;
    }
    }
    return true;
};

void InvestTechnologyCompanyCommand::execute(GameState* state, GameController* controller) {
    if(m_sourcePlayer->getCoins()==0){
        m_isFailed=true;
        m_failureMessage="没钱投资了。";
        return ;
    }

    //读取选项
    int value=m_userInput[0];

    //不投资，不产生日志
    if(m_sourcePlayer->getCoins()==0){
        m_isFailed=true;
        m_failureMessage="选择不投资。";
        return;
    }

    //投资
    m_sourcePlayer->delCoins(1);
    m_card->changeValue(1);
    m_coinsSum=m_card->getValue();
}

QString InvestTechnologyCompanyCommand::getLog()const {
    QString log=QString("【%1】 %2 ").arg(m_card->getName()).arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else
        log+=QString("投资了 1 金币 ，目前共投资了 %1 金币！").arg(m_coinsSum);

    return log;
}

