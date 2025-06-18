#include "rolldicecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "dice.h"

RollDiceCommand::RollDiceCommand(Player* player, QObject* parent):GameCommand(CommandType::RollDice,player,parent){

};


// 检查是否需要用户交互（默认不需要交互）
bool RollDiceCommand::requiresUserInput() const { return true; };

// 获得选项
QVariantMap RollDiceCommand::getPromptData() const {
    QVariantMap promptData;
    promptData["commandId"] = m_id;
    promptData["commandType"] = static_cast<int>(CommandType::RollDice);
    promptData["sourcePlayerName"] = m_sourcePlayer ? m_sourcePlayer->getName() : "未知玩家";
    promptData["promptMessage"] = "请选择抛掷一个骰子还是两个骰子。"; // 完整提示文本
    promptData["inputType"] = "diceRollChoice"; // 提示UI这是骰子选择类型

    QVariantList choices;
    // 选项1：抛一个
    QVariantMap choice1;
    choice1["value"] = 1;
    choice1["text"] = "抛一个";
    choice1["enabled"] = true; // 总是可选
    choices.append(choice1);

    // 选项2：抛两个 (根据火车站条件)
    QVariantMap choice2;
    choice2["value"] = 2;
    choice2["text"] = "抛两个";
    choice2["enabled"] = (m_sourcePlayer->getCardNum("火车站",State::Opening)==1);
    choices.append(choice2);

    promptData["choices"] = choices; // 完整的选项列表，包括启用状态

    return promptData;
};

// 获取默认选项
QVariantMap RollDiceCommand::getAutoChoice( QVariantMap& promptData ,GameState* state) const {
    QVariantMap choice;
    //先假设只抛一个
    choice["value"]=1;
    return choice;
};

void RollDiceCommand::execute(GameState* state, GameController* controller){
    //读取选项
    int diceNum = m_userChoice.value("value", 1).toInt();
    //抛骰子
    Dice* dice=state->getDice();
    dice->rollDice(diceNum);
    //保存骰子结果
    m_diceNum1=dice->getFirstNum();
    m_diceNum2=dice->getSecondNum();
};

QString RollDiceCommand::getLog() const {
    QString log=QString("%1 投掷了骰子，结果为： ").arg(m_sourcePlayer->getName());

    if(m_diceNum2==0)
        log+=QString("%1。").arg(m_diceNum1);
    else
        log+=QString("%1+%2=%3").arg(m_diceNum1).arg(m_diceNum2).arg(m_diceNum1+m_diceNum2);

    return log;
};
