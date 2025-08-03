#include "wheatfield.h"

WheatField::WheatField(QObject* parent):
    Card("麦田", 1, Color::Blue, Type::Agriculture, 1 , 1, 1 ,CommandType::GainCoins,parent) {}

QString WheatField::getDescription() const {
    return QString("获得 %1 金币。").arg(this->m_value);
}
