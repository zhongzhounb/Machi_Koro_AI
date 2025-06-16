#include "appleorchard.h"
#include "commandfactory.h"

AppleOrchard::AppleOrchard(QObject* parent):
    Card("果园", 3, Color::Blue, Type::Agriculture, 3, 10, 10, parent) {}

QString AppleOrchard::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}

double AppleOrchard::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> AppleOrchard::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsCommand(owner,this,this)};
}
