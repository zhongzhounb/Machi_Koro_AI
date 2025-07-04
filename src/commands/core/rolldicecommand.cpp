#include "rolldicecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"
#include "commandfactory.h"

RollDiceCommand::RollDiceCommand(Player* sourcePlayer,  QObject* parent)
    : GameCommand(CommandType::RollDice, sourcePlayer,parent){
}


// 检查是否需要用户交互（默认不需要交互）
bool RollDiceCommand::requiresUserInput() const { return true; };


void RollDiceCommand::execute(GameState* state, GameController* controller){
    //读取选项
    QVariantList chooes=m_userChoice.value("valueList").toList();;
    int diceNum=chooes.at(0).toInt();
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
