#ifndef CARDFACTORY_H
#define CARDFACTORY_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QList> // 新增，因为 createCards 会返回 QList

#include "global.h" // 假设 Card 基类和 Type 枚举在这里定义

// 前向声明 Card 基类
class Card;

// 定义所有卡牌类型的枚举 (作为内部标识符)
enum CardType {
    // 蓝色卡牌 (Blue Cards)
    Card_AppleOrchard,
    Card_FlowerOrchard,
    Card_Forest,
    Card_MackerelBoat,
    Card_Ranch,
    Card_TunaBoat,
    Card_WheatField,

    // 绿色卡牌 (Green Cards)
    Card_Bakery,
    Card_CheeseFactory,
    Card_DemolitionCompany,
    Card_Flowershop,
    Card_FruitAndVegetableMarket,
    Card_FurnitureFactory,
    Card_GeneralStore,
    Card_MovingCompany,
    Card_SodaBottlingPlant,

    // 地标卡牌 (Landmark Cards)
    Card_Airport,
    Card_AmusementPark,
    Card_Harbor,
    Card_RadioTower,
    Card_ShoppingMall,
    Card_Station,

    // 紫色卡牌 (Purple Cards)
    Card_BusinessCenter,
    Card_Publisher,
    Card_Stadium,
    Card_TaxOffice,
    Card_TechStartup,

    // 红色卡牌 (Red Cards)
    Card_Cafe,
    Card_FamilyRestaurant,
    Card_FrenchRestaurant,
    Card_MembersOnlyClub,
    Card_PizzaJoint,
    Card_Sushibar,

    // 用于错误处理或未指定类型的默认值
    Card_None = -1
};

class CardFactory {
public:
    // 单例模式，获取工厂实例
    static CardFactory& instance();

    // 根据卡牌名称（QString）创建单个卡牌实例
    Card* createCard(const QString& cardName, QObject* parent = nullptr);

    // 根据卡牌名称和数量创建多个卡牌实例
    QList<Card*> createCards(const QString& cardName, int count, QObject* parent = nullptr);

private:
    // 私有构造函数和删除拷贝构造/赋值运算符，确保单例模式
    CardFactory(); // 构造函数现在需要初始化映射
    CardFactory(const CardFactory&) = delete;
    CardFactory& operator=(const CardFactory&) = delete;

    // 内部映射：卡牌名称字符串 -> 卡牌类型枚举
    static QMap<QString, CardType> m_cardNameToTypeMap;

    // 辅助方法：初始化卡牌名称到类型的映射
    void initializeCardMap();

    // 辅助方法：根据 CardType 创建单个卡牌实例，避免重复代码
    Card* createCardInternal(CardType type, QObject* parent);
};

#endif // CARDFACTORY_H
