#include "cardfactory.h"
#include <QDebug> // 用于错误日志

// 包含所有具体卡牌类的头文件
// 请确保这些路径和文件名与你的项目结构完全匹配

// 蓝色卡牌 (Blue Cards)
#include "cards/blue/appleorchard.h"
#include "cards/blue/flowerorchard.h"
#include "cards/blue/forest.h"
#include "cards/blue/mackerelboat.h"
#include "cards/blue/ranch.h"
#include "cards/blue/tunaboat.h"
#include "cards/blue/wheatfield.h"

// 绿色卡牌 (Green Cards)
#include "cards/green/bakery.h"
#include "cards/green/cheesefactory.h"
#include "cards/green/demolitioncompany.h"
#include "cards/green/flowershop.h"
#include "cards/green/fruitandvegetablemarket.h"
#include "cards/green/furniturefactory.h"
#include "cards/green/generalstore.h"
#include "cards/green/movingcompany.h"
#include "cards/green/sodabottlingplant.h"

// 地标卡牌 (Landmark Cards)
#include "cards/landmark/airport.h"
#include "cards/landmark/amusementpark.h"
#include "cards/landmark/harbor.h"
#include "cards/landmark/radiotower.h"
#include "cards/landmark/shoppingmall.h"
#include "cards/landmark/station.h"

// 紫色卡牌 (Purple Cards)
#include "cards/purple/businesscenter.h"
#include "cards/purple/publisher.h"
#include "cards/purple/stadium.h"
#include "cards/purple/taxoffice.h"
#include "cards/purple/techstartup.h"

// 红色卡牌 (Red Cards)
#include "cards/red/cafe.h"
#include "cards/red/familyrestaurant.h"
#include "cards/red/frenchrestaurant.h"
#include "cards/red/membersonlyclub.h"
#include "cards/red/pizzajoint.h"
#include "cards/red/sushibar.h"


// 初始化静态成员变量
QMap<QString, CardType> CardFactory::m_cardNameToTypeMap;

// 构造函数：初始化卡牌名称到类型的映射
CardFactory::CardFactory() {
    initializeCardMap();
}

// 实现单例模式的实例获取方法
CardFactory& CardFactory::instance() {
    static CardFactory factory;
    return factory;
}

// 初始化卡牌名称到类型的映射
void CardFactory::initializeCardMap() {
    if (!m_cardNameToTypeMap.isEmpty()) {
        return; // 已经初始化过，避免重复
    }

    m_cardNameToTypeMap["果园"] = Card_AppleOrchard;
    m_cardNameToTypeMap["花田"] = Card_FlowerOrchard;
    m_cardNameToTypeMap["林场"] = Card_Forest;
    m_cardNameToTypeMap["鲭鱼船"] = Card_MackerelBoat;
    m_cardNameToTypeMap["农场"] = Card_Ranch;
    m_cardNameToTypeMap["金枪鱼船"] = Card_TunaBoat;
    m_cardNameToTypeMap["麦田"] = Card_WheatField;

    // 绿色卡牌 (Green Cards)
    m_cardNameToTypeMap["面包房"] = Card_Bakery;
    m_cardNameToTypeMap["奶酪工厂"] = Card_CheeseFactory;
    m_cardNameToTypeMap["拆迁公司"] = Card_DemolitionCompany;
    m_cardNameToTypeMap["花店"] = Card_Flowershop;
    m_cardNameToTypeMap["果蔬超市"] = Card_FruitAndVegetableMarket;
    m_cardNameToTypeMap["家具工厂"] = Card_FurnitureFactory;
    m_cardNameToTypeMap["杂货店"] = Card_GeneralStore;
    m_cardNameToTypeMap["搬家公司"] = Card_MovingCompany;
    m_cardNameToTypeMap["饮料工厂"] = Card_SodaBottlingPlant;

    // 地标卡牌 (Landmark Cards)
    m_cardNameToTypeMap["机场"] = Card_Airport;
    m_cardNameToTypeMap["游乐园"] = Card_AmusementPark;
    m_cardNameToTypeMap["港口"] = Card_Harbor;
    m_cardNameToTypeMap["广播塔"] = Card_RadioTower;
    m_cardNameToTypeMap["购物中心"] = Card_ShoppingMall;
    m_cardNameToTypeMap["火车站"] = Card_Station;

    // 紫色卡牌 (Purple Cards)
    m_cardNameToTypeMap["商业中心"] = Card_BusinessCenter;
    m_cardNameToTypeMap["出版社"] = Card_Publisher;
    m_cardNameToTypeMap["体育馆"] = Card_Stadium;
    m_cardNameToTypeMap["税务所"] = Card_TaxOffice;
    m_cardNameToTypeMap["科技公司"] = Card_TechStartup;

    // 红色卡牌 (Red Cards)
    m_cardNameToTypeMap["咖啡馆"] = Card_Cafe;
    m_cardNameToTypeMap["西餐厅"] = Card_FamilyRestaurant;
    m_cardNameToTypeMap["法国餐厅"] = Card_FrenchRestaurant;
    m_cardNameToTypeMap["会员俱乐部"] = Card_MembersOnlyClub;
    m_cardNameToTypeMap["披萨店"] = Card_PizzaJoint;
    m_cardNameToTypeMap["寿司店"] = Card_Sushibar;
}

// 辅助方法：根据 CardType 创建单个卡牌实例，避免重复代码
Card* CardFactory::createCardInternal(CardType type, QObject* parent) {
    switch (type) {
    // 蓝色卡牌 (Blue Cards)
    case Card_AppleOrchard: return new AppleOrchard(parent);
    case Card_FlowerOrchard: return new FlowerOrchard(parent);
    case Card_Forest: return new Forest(parent);
    case Card_MackerelBoat: return new MackerelBoat(parent);
    case Card_Ranch: return new Ranch(parent);
    case Card_TunaBoat: return new TunaBoat(parent);
    case Card_WheatField: return new WheatField(parent);

        // 绿色卡牌 (Green Cards)
    case Card_Bakery: return new Bakery(parent);
    case Card_CheeseFactory: return new CheeseFactory(parent);
    case Card_DemolitionCompany: return new DemolitionCompany(parent);
    case Card_Flowershop: return new FlowerShop(parent);
    case Card_FruitAndVegetableMarket: return new FruitAndVegetableMarket(parent);
    case Card_FurnitureFactory: return new FurnitureFactory(parent);
    case Card_GeneralStore: return new GeneralStore(parent);
    case Card_MovingCompany: return new MovingCompany(parent);
    case Card_SodaBottlingPlant: return new SodaBottlingPlant(parent);

        // 地标卡牌 (Landmark Cards)
    case Card_Airport: return new Airport(parent);
    case Card_AmusementPark: return new AmusementPark(parent);
    case Card_Harbor: return new Harbor(parent);
    case Card_RadioTower: return new RadioTower(parent);
    case Card_ShoppingMall: return new ShoppingMall(parent);
    case Card_Station: return new Station(parent);

        // 紫色卡牌 (Purple Cards)
    case Card_BusinessCenter: return new BusinessCenter(parent);
    case Card_Publisher: return new Publisher(parent);
    case Card_Stadium: return new Stadium(parent);
    case Card_TaxOffice: return new TaxOffice(parent);
    case Card_TechStartup: return new TechStartup(parent);

        // 红色卡牌 (Red Cards)
    case Card_Cafe: return new Cafe(parent);
    case Card_FamilyRestaurant: return new FamilyRestaurant(parent);
    case Card_FrenchRestaurant: return new FrenchRestaurant(parent);
    case Card_MembersOnlyClub: return new MembersOnlyClub(parent);
    case Card_PizzaJoint: return new PizzaJoint(parent);
    case Card_Sushibar: return new SushiBar(parent);

    case Card_None:
    default:
        qWarning("CardFactory: Unhandled card type %d during internal creation. Returning nullptr.", static_cast<int>(type));
        return nullptr;
    }
}

// 实现 createCard 方法，根据传入的卡牌名称创建对应的卡牌实例
Card* CardFactory::createCard(const QString& cardName, QObject* parent) {
    if (!m_cardNameToTypeMap.contains(cardName)) {
        qWarning("CardFactory: Unknown card name '%s'. Returning nullptr.", qPrintable(cardName));
        return nullptr;
    }
    CardType type = m_cardNameToTypeMap.value(cardName);
    return createCardInternal(type, parent);
}

// 实现 createCards 方法，根据传入的卡牌名称和数量创建多个卡牌实例
QList<Card*> CardFactory::createCards(const QString& cardName, int count, QObject* parent) {
    QList<Card*> cards;

    if (count <= 0) {
        qWarning("CardFactory: Requested to create %d cards. Returning empty list.", count);
        return cards; // 返回空列表
    }

    if (!m_cardNameToTypeMap.contains(cardName)) {
        qWarning("CardFactory: Unknown card name '%s'. Returning empty list.", qPrintable(cardName));
        return cards; // 返回空列表
    }

    CardType type = m_cardNameToTypeMap.value(cardName);

    for (int i = 0; i < count; ++i) {
        Card* newCard = createCardInternal(type, parent);
        if (newCard) {
            cards.append(newCard);
        } else {
            // 如果在创建过程中失败，记录警告并停止
            qWarning("CardFactory: Failed to create card of type %d for name '%s' during batch creation at index %d. Stopping.",
                     static_cast<int>(type), qPrintable(cardName), i);
            // 可以选择在这里清理已创建的卡牌，但通常返回部分成功的列表
            break;
        }
    }
    return cards;
}
