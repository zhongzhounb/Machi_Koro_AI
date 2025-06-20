#include "rerolldicecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"

RerollDiceCommand::RerollDiceCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::GainCoins, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

// 检查是否需要用户交互（可选交互：如果自己的地标都不够拆，直接全拆了，不用选择）
bool RerollDiceCommand::requiresUserInput()const {
    if(m_sourcePlayer->getCardNum(m_card->getName(),State::Opening)>=m_sourcePlayer->getTypeCardNum(Type::Landmark,State::Opening))
        return false;
    return true;
};

void RerollDiceCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    QVariantList chooes=m_userChoice.value("valueList").toList();;
    int diceNum=chooes.at(0).toInt();
    //如果选择跳过则不触发该命令
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


