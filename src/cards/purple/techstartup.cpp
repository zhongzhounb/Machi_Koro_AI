#include "techstartup.h"
#include "commandfactory.h"

TechStartup::TechStartup(QObject* parent):
    Card("科技公司", 1, Color::Purple, Type::Office, 0 , 10, 10, parent) {}

QString TechStartup::getDescription() const {
    return QString("若你是投掷者，收取每人 %1 金币。你每回合可投资1金币，使所收的金币+1。").arg(m_value);
}

double TechStartup::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> TechStartup::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    //如果还没投资
    if(m_value==0)
        return {CommandFactory::instance().createStealCoinsFromAllCommand(owner,this,this,true,"还没开张呢。")};
    return {CommandFactory::instance().createStealCoinsFromAllCommand(owner,this,this)};
}
