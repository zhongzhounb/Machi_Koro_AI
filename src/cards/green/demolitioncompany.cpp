#include "demolitioncompany.h"
#include "commandfactory.h"

DemolitionCompany::DemolitionCompany(QObject* parent):
    Card("拆迁公司", 2, Color::Green, Type::Company, 8 , 4, 4, parent) {}

QString DemolitionCompany::getDescription() const {
    return QString("若你是投掷者，且有已建成的地标，必须选择一个拆除，然后获得 %1 金币").arg(m_value);
}

double DemolitionCompany::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> DemolitionCompany::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createGainCoinsCommand(owner,this,this)};
}
