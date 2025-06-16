#include "flowerorchard.h"
#include "commandfactory.h"

FlowerOrchard::FlowerOrchard(QObject* parent):
    Card("花田", 2, Color::Blue, Type::Agriculture, 1, 4, 4,parent) {}

QString FlowerOrchard::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}

double FlowerOrchard::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> FlowerOrchard::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsCommand(owner,this,this)};
}
