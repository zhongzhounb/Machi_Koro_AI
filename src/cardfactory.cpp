#include "cardfactory.h"
#include "cards/wheatfield.h"
CardFactory::CardFactory(QObject* parent) : QObject(parent)
{
}

CardFactory::~CardFactory(){}

Card* CardFactory::createCardInstanceById(int cardId, QObject* parent) {
    switch (cardId) {
    case 101: return new WheatField(parent);
    // ... 添加所有22种卡牌的创建逻辑 ...
    default:
        qWarning() << "CardFactory: Unknown card ID for instance creation:" << cardId;
        return nullptr;
    }
}
