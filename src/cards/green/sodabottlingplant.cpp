#include "sodabottlingplant.h"
#include "commandfactory.h"
#include "player.h"
#include "gamestate.h"
SodaBottlingPlant::SodaBottlingPlant(QObject* parent):
    Card("饮料工厂", 5, Color::Green, Type::Factory, 1 , 11, 11, parent),m_comboType(Type::Restaurant) {}

QString SodaBottlingPlant::getDescription() const {
    return QString("若你是投掷者，所有人每拥有一个%1，你获得 %2 金币。").arg(typeToImg(m_comboType)).arg(m_value);
}

int SodaBottlingPlant::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=0;
    for(Player* player:gameState->getPlayers())
        num+=player->getTypeCardNum(m_comboType,State::None);
    return num;
}


double SodaBottlingPlant::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> SodaBottlingPlant::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsComboTypeCommand(owner,this,m_comboType,false,this)};
}
