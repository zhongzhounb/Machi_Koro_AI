#include "businesscenter.h"

BusinessCenter::BusinessCenter(QObject* parent):
    Card("商业中心", 8, Color::Purple, Type::Office, -1 , 6, 6,CommandType::SwapCard, parent) {}

QString BusinessCenter::getDescription() const {
    return QString("若你是投掷者，必须与一名玩家交换一张非%1建筑。").arg(typeToImg(Type::Office));
}
