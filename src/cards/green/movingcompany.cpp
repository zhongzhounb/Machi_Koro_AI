#include "movingcompany.h"
#include "commandfactory.h"
#include "player.h"

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
    //唯一特殊卡：是绿卡且强制主动技能，说明可能一种卡有多个主动技（紫卡和地标限制只用一个），用List就是用来处理他的，真是为了一盘醋包的饺子！
    QList<GameCommand*> commands;
    int cardNum=owner->getCardNum(m_name,State::Opening);
    for(int i=0;i<cardNum;i++)
        commands.append(CommandFactory::instance().createGiveCardCommand(owner,this,this));
    return commands;
}
