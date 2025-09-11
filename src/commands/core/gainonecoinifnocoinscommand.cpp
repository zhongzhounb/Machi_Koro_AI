#include "gainonecoinifnocoinscommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "commandfactory.h"
#include "dice.h"

GainOneCoinIfNoCoinsCommand::GainOneCoinIfNoCoinsCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::GainOneCoinIfNoCoins, sourcePlayer,parent,cards,activePlayer){
}

void GainOneCoinIfNoCoinsCommand::execute(GameState* state, GameController* controller) {

    //将要购买，新建机场命令
    addCommand("机场",controller);

    //清空骰子,给不购买按钮腾出位置
    Dice* dice=state->getDice();
    dice->clearDice();

    //如果大于0，不执行
    if(m_sourcePlayer->getCoins()>0)
        return;

    //赚钱
    m_sourcePlayer->addCoins(1);

    //日志
    QString log=QString(" %1 因为没钱，领了 1 金币的低保。").arg(m_sourcePlayer->getName());
    state->addLog(log);

}
