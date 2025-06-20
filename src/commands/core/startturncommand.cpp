#include "startturncommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

StartTurnCommand::StartTurnCommand(Player* sourcePlayer,  QObject* parent)
    : GameCommand(CommandType::GainCoins, sourcePlayer,parent){
}

void StartTurnCommand::execute(GameState* state, GameController* controller) {
    /*创建基本命令到队列*/
    //抛骰子
}

QString StartTurnCommand::getLog()const {
    return QString("========玩家 %1 的回合========").arg(m_sourcePlayer->getName());
}
