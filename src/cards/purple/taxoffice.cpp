#include "taxoffice.h"
#include "commandfactory.h"

TaxOffice::TaxOffice(QObject* parent):
    Card("税务所", 4, Color::Purple, Type::Office, 2 , 8, 9, parent) {}

QString TaxOffice::getDescription() const {
    return QString("若你是投掷者，其他人若有≥10金币，收取其一半（向下取整）金币。").arg(m_value);
}

double TaxOffice::getBuyWight(Player* aiPlayer, GameState* gameState) const {
    return 0.0;
}

QList<GameCommand*> TaxOffice::createCommands(Player* owner, Player* activePlayer){
    Q_UNUSED(activePlayer);
    return {CommandFactory::instance().createStealCoinsHalfFromWealthyCommand(owner,this,this)};
}
