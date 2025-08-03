#include "techstartup.h"

TechStartup::TechStartup(QObject* parent):
    Card("科技公司", 1, Color::Purple, Type::Office, 0 , 10, 10,CommandType::StealCoinsFromAll, parent
           ,""
           ,""
           ,State::Opening
           ,CommandType::InvestTechnologyCompany) {}

QString TechStartup::getDescription() const {
    return QString("若你是投掷者，收取每人 %1 金币。你每回合可投资1金币，使所收的金币+1。").arg(m_value);
}
