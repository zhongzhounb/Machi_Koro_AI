#include "bakery.h"
#include "commandfactory.h"

Bakery::Bakery(QObject* parent):
    Card("面包房", 1, Color::Green, Type::Store, 1 , 2, 3, parent) {}

QString Bakery::getDescription() const {
    return QString("若你是投掷者，获得 %1 金币。").arg(m_value);
}

double Bakery::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> Bakery::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsCommand(owner,this,this)};
}
