#include "adddicenumcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"

AddDiceNumCommand::AddDiceNumCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::AddDiceNum, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
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

void AddDiceNumCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    QVariantList choose=m_userChoice.value("valueList").toList();
    m_isAdd=choose.at(0).toBool();

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
