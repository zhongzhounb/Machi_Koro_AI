#include "publisher.h"
#include "commandfactory.h"

Publisher::Publisher(QObject* parent):
    Card("出版社", 5, Color::Purple, Type::Office, 1 , 7, 7, parent) {}

QString Publisher::getDescription() const {
    return QString("若你是投掷者，其他人每拥有一个%1/%2，收取其 1 金币。").arg(typeToImg(Type::Restaurant).arg(typeToImg(Type::Store)));
}

double Publisher::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> Publisher::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createStealCoinsComboRedStoreCommand(owner,this,this)};
}
