#include "sodabottlingplant.h"
#include "player.h"
#include "gamestate.h"
SodaBottlingPlant::SodaBottlingPlant(QObject* parent):
    Card("饮料工厂", 5, Color::Green, Type::Factory, 1 , 11, 11,CommandType::GainCoins, parent
           ,"场上共有%1个"+typeToImg(Type::Restaurant)) {}

QString SodaBottlingPlant::getDescription() const {
    return QString("若你是投掷者，所有人每拥有一个%1，你获得 %2 金币。").arg(typeToImg(Type::Restaurant)).arg(m_value);
}

int SodaBottlingPlant::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=0;
    for(Player* player:gameState->getPlayers())
        num+=player->getTypeCardNum(Type::Restaurant,State::None);
    return num;
}
