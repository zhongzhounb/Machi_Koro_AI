#include "initgamecommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "global.h" // 确保包含全局变量 Initialized 的声明

InitGameCommand::InitGameCommand(Player* sourcePlayer, QObject* parent, QList<Card*> cards, Player* activePlayer)
    : GameCommand(CommandType::InitGame, sourcePlayer, parent, cards, activePlayer) {
}

// InitGameCommand.cpp

// 第一次：告知前端开黑幕
PromptData InitGameCommand::getPromptData(GameState* state) const {
    PromptData pd;
    pd.type = PromptData::LoadingAnimation;
    switch (m_currentStep){
    case 1:{
        pd.autoInput=1;
        return pd;
    }
    case 2:{
        pd.autoInput=2;
        return pd;
    }
    }
    return pd;
}

// 接收回调
bool InitGameCommand::setInput(int optionId, GameState* state, GameController* controller) {
    switch (m_currentStep){
    case 1:{
        execute(state,controller);
        m_currentStep=2;
        return false;
    }
    case 2:{
        return true;
    }
    }
    return true;

}

void InitGameCommand::execute(GameState* state, GameController* controller) {
    /* 核心：逻辑锁重置 */
    // 必须先设为 false，防止上一局游戏的残留状态导致动画瞬间消失
    //Initialized = false;

    qDebug() << "InitGameCommand: 开始初始化游戏数据...";

    /* 游戏初始设置 */
    // 这里会执行你贴出的那些 initState 代码（加牌、洗牌、设置商店等）
    state->initState();

    // 可以在这里插入日志
    QString log = QString("欢迎来到骰子街，游戏开始！");
    state->addLog(log);

    /* 核心：逻辑锁解除 */
    // 此时 initState 已跑完，前端的 pollTimer 会在下次轮询（100ms内）发现此变化
    //Initialized = true;

    qDebug() << "InitGameCommand: 初始化完成，逻辑锁已解除。";
}
