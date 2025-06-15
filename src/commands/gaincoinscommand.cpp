#include "gaincoinscommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"      // 需要 GameState
#include "gamecontroller.h" // 需要 GameController

// 用于反序列化的默认构造
GainCoinsCommand::GainCoinsCommand():GameCommand(CommandType::None, nullptr, nullptr, nullptr, nullptr,false, QString()) {
    m_coins=0;
}

GainCoinsCommand::GainCoinsCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::GainCoins, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
    m_coins=0;
}

void GainCoinsCommand::execute(GameState* state, GameController* controller) {

}

