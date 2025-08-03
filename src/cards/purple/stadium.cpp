#include "stadium.h"

Stadium::Stadium(QObject* parent):
    Card("体育馆", 6, Color::Purple, Type::Office, 2 , 6, 6,CommandType::StealCoinsFromAll, parent) {}

QString Stadium::getDescription() const {
    return QString("若你是投掷者，收取每人 %1 金币。").arg(m_value);
}
