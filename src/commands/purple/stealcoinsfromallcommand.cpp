#include "stealcoinsfromallcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "logutils.h"

StealCoinsFromAllCommand::StealCoinsFromAllCommand(Player* sourcePlayer, QObject* parent,QList<Card*> cards,Player* activePlayer)
    : GameCommand(CommandType::StealCoinsFromAll, sourcePlayer,parent,cards,activePlayer){
}

void StealCoinsFromAllCommand::execute(GameState* state, GameController* controller) {
    QString log=QString("【%1】 %2 ").arg(m_card->getName())
                      .arg(m_sourcePlayer->getName());
    int coinsSum=0;
    bool isFirst=true;
    for(Player* player:state->getPlayers())
        if(player!=m_sourcePlayer){
            int coins=qMin(m_card->getStealCoins(player),player->getCoins());
            coinsSum+=coins;
            //赚钱
            m_sourcePlayer->stealCoins(player,coins);
            if(coins>0){
                if(isFirst){
                    log+="偷了";
                    isFirst=true;
                }
                else
                    log+=",";
                log+=QString("%1 %2 金币").arg(player->getName()).arg(coins);
            }
        }
    if(coinsSum==0)
        log+="没偷到任何人的金币。";
    else
        log+=QString("共偷到 %1 金币。").arg(coinsSum);
    state->addLog(log);
}


