#include "fruitandvegetablemarket.h"
#include "commandfactory.h"

FruitAndVegetableMarket::FruitAndVegetableMarket(QObject* parent):
    Card("果蔬超市", 2, Color::Green, Type::Factory, 2 , 11, 12, parent),m_comboType(Type::Agriculture) {}

QString FruitAndVegetableMarket::getDescription() const {
    return QString("若你是投掷者，你每拥有一个%1，获得 %2 金币。").arg(typeToImg(m_comboType)).arg(m_value);
}

double FruitAndVegetableMarket::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> FruitAndVegetableMarket::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsComboTypeCommand(owner,this,m_comboType,this)};
}
