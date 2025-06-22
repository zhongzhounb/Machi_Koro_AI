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

void RerollDiceCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    QVariantList chooes=m_userChoice.value("valueList").toList();;
    int diceNum=chooes.at(0).toInt();
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


