#include "ranch.h"
#include "commandfactory.h"

Ranch::Ranch(QObject* parent):
    Card("农场", 1, Color::Blue, Type::Husbandry, 1 , 2, 2 ,parent) {}

QString Ranch::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}

double Ranch::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> Ranch::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsCommand(owner,this,this)};
}
