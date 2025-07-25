#include "businesscenter.h"
#include "commandfactory.h"

BusinessCenter::BusinessCenter(QObject* parent):
    Card("商业中心", 8, Color::Purple, Type::Office, -1 , 6, 6, parent) {}

QString BusinessCenter::getDescription() const {
    return QString("若你是投掷者，必须与一名玩家交换一张非%1建筑。").arg(typeToImg(Type::Office));
}

double BusinessCenter::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> BusinessCenter::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createSwapCardCommand(owner,this,this)};
}
