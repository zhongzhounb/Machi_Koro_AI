#include "cheesefactory.h"
#include "commandfactory.h"
#include "player.h"
CheeseFactory::CheeseFactory(QObject* parent):
    Card("奶酪工厂", 5, Color::Green, Type::Factory, 3 , 7, 7,parent),m_comboType(Type::Husbandry) {}

QString CheeseFactory::getDescription() const {
    return QString("若你是投掷者，你每拥有一个%1，获得 %2 金币。").arg(typeToImg(m_comboType)).arg(m_value);
}

int CheeseFactory::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getTypeCardNum(m_comboType,State::None);
    return num;
}


double CheeseFactory::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> CheeseFactory::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsComboTypeCommand(owner,this,m_comboType,this)};
}
