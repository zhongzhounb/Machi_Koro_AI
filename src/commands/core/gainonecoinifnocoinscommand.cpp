#include "gainonecoinifnocoinscommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

GainOneCoinIfNoCoinsCommand::GainOneCoinIfNoCoinsCommand(Player* sourcePlayer,  QObject* parent)
    : GameCommand(CommandType::GainOneCoinIfNoCoins, sourcePlayer,parent){
}

void GainOneCoinIfNoCoinsCommand::execute(GameState* state, GameController* controller) {
    //如果大于0，不执行
    if(m_sourcePlayer->getCoins()>0)
        return;

    //赚钱
    m_sourcePlayer->addCoins(1);

    //新建命令
    if(m_sourcePlayer->getCardNum("机场",State::Opening)>0)
        controller->addCommand(m_sourcePlayer->getCardSpecialCommand("机场"));

    //日志
    QString log=QString(" %1 因为没钱，领了 1 金币的低保。").arg(m_sourcePlayer->getName());
    state->addLog(log);

}
