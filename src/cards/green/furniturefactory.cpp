#include "furniturefactory.h"
#include "commandfactory.h"

FurnitureFactory::FurnitureFactory(QObject* parent):
    Card("家具工厂", 3, Color::Green, Type::Factory, 3 , 8, 8, parent),m_comboType(Type::Industry) {}


QString FurnitureFactory::getDescription() const {
    return QString("若你是投掷者，你每拥有一个%1，获得 %2 金币。").arg(typeToImg(m_comboType)).arg(m_value);
}

double FurnitureFactory::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> FurnitureFactory::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsComboTypeCommand(owner,this,m_comboType,this)};
}
