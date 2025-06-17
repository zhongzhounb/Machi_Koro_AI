#include "rolldicecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

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
QVariantMap RollDiceCommand::getAutoChoice( QVariantMap& promptData ,GameState* state) const override;

void RollDiceCommand::execute(GameState* state, GameController* controller=nullptr) override;

QString RollDiceCommand::getLog() const override;
