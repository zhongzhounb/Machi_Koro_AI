#include "taxoffice.h"
#include "player.h"

TaxOffice::TaxOffice(QObject* parent):
    Card("税务所", 4, Color::Purple, Type::Office, 2 , 8, 9,CommandType::StealCoinsFromAll, parent) {}

QString TaxOffice::getDescription() const {
    return QString("若你是投掷者，其他人若有≥10金币，收取其一半（向下取整）金币。").arg(m_value);
}

int TaxOffice::getStealCoins(Player* player)const{
    int num=player->getCoins();
    if(num>=10)
        return num/2;
    return 0;
}
