#include "flowershop.h"
#include "commandfactory.h"
#include "player.h"
FlowerShop::FlowerShop(QObject* parent):
    Card("花店", 1, Color::Green, Type::Store, 1 , 6, 6, parent),m_comboName("花田") {}

QString FlowerShop::getDescription() const {
    return QString("若你是投掷者，你每拥有一个【%1】，获得 %2 金币。").arg(m_comboName).arg(m_value);
}

int FlowerShop::getComboNum(Player* owner, Player* activePlayer,GameState* gameState)const{
    int num=owner->getCardNum(m_comboName,State::None);
    return num;
}

double FlowerShop::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> FlowerShop::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsComboNameCommand(owner,this,m_comboName,this)};
}

