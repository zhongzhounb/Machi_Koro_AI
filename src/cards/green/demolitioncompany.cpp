#include "demolitioncompany.h"

DemolitionCompany::DemolitionCompany(QObject* parent):
    Card("拆迁公司", 2, Color::Green, Type::Company, 8 , 4, 4,CommandType::CloseLandmark,parent) {}

QString DemolitionCompany::getDescription() const {
    return QString("若你是投掷者且建成了地标，必须选择一个拆除，然后获得 %1 金币").arg(m_value);
}
