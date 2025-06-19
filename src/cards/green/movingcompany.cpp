#include "movingcompany.h"
#include "commandfactory.h"

MovingCompany::MovingCompany(QObject* parent):
    Card("搬家公司", 2, Color::Green, Type::Company, 4 , 9, 10, parent) {}

QString MovingCompany::getDescription() const {
    return QString("若你是投掷者，必须选择一个非%1标志的卡（可以是此卡），给任意一个玩家，然后获得 %2 金币").arg(typeToImg(Type::Office)).arg(m_value);
}

double MovingCompany::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> MovingCompany::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsCommand(owner,this,this)};
}
