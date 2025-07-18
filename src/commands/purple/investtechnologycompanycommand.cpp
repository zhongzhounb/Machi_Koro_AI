#include "investtechnologycompanycommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

InvestTechnologyCompanyCommand::InvestTechnologyCompanyCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::InvestTechnologyCompany, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

// 检查是否需要用户交互（可选交互：如果自己的地标都不够拆，直接全拆了，不用选择）
PromptData CloseLandmarkCommand::getPromptData(GameState* state) const {

};
// 获取默认选项（无选项时禁止调用）
int CloseLandmarkCommand::getAutoInput( const PromptData& promptData ,GameState* state) const {

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool CloseLandmarkCommand::setInput(int optionId,GameState* state) {

};

void InvestTechnologyCompanyCommand::execute(GameState* state, GameController* controller) {
    if(m_sourcePlayer->getCoins()==0){
        m_isFailed=true;
        m_failureMessage="没钱投资了。";
        return ;
    }

    //读取选项
    QVariantList chooes=m_userChoice.value("valueList").toList();
    int value=chooes.indexOf(0);

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

