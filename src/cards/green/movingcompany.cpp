#include "movingcompany.h"
#include "player.h"

MovingCompany::MovingCompany(QObject* parent):
    Card("搬家公司", 2, Color::Green, Type::Company, 4 , 9, 10,CommandType::GiveCard, parent) {}

QString MovingCompany::getDescription() const {
    return QString("若你是投掷者，必须选一个非%1建筑给予另一个玩家，然后获得 %2 金币").arg(typeToImg(Type::Office)).arg(m_value);
}
