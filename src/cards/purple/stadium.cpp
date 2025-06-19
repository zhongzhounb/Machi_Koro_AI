#include "stadium.h"
#include "commandfactory.h"

Stadium::Stadium(QObject* parent):
    Card("体育馆", 6, Color::Purple, Type::Office, 2 , 6, 6, parent) {}

QString Stadium::getDescription() const {
    return QString("若你是投掷者，收取每人 %1 金币。").arg(m_value);
}

double Stadium::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> Stadium::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createStealCoinsFromAllCommand(owner,this,this)};
}
