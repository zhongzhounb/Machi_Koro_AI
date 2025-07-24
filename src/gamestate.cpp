#include "gamestate.h"
#include "player.h"
#include "dice.h"      // 假设有 Dice 类
#include "card.h"      // 需要 Card 类定义
#include "cardstore.h"
#include "cardfactory.h"

GameState::GameState(QObject* parent)
    : QObject(parent)
    , m_dice(nullptr) // 初始为 nullptr
    ,m_currentPlayer(nullptr)// 初始为 nullptr
{
    //先创建骰子
    m_dice = new Dice(this);
    //再创建商店
    m_cardStores.append(new CardStore(1,5,Color::BackSmall,this));
    m_cardStores.append(new CardStore(2,5,Color::BackBig,this));
    m_cardStores.append(new CardStore(3,2,Color::BackPurple,this));
    //假设有五个人，等主要工作做好会将以下部分给command
    addPlayer(new Player("仲舟",AIRank::Easy,this));
    addPlayer(new Player("ghost_him",AIRank::Easy,this));
    addPlayer(new Player("南海十三郎",AIRank::Easy,this));
    addPlayer(new Player("兔小兔",AIRank::Easy,this));
    addPlayer(new Player("wisdon",AIRank::Easy,this));
}

GameState::~GameState() {

}

QList<Card*> GameState::getPlayerInitCards(){
    QList<Card*> cards;
    //初始普通卡
    cards.append(CardFactory::instance().createCard("麦田", this));
    cards.append(CardFactory::instance().createCard("面包房", this));
    /*cards.append(CardFactory::instance().createCard("果园", this));
    cards.append(CardFactory::instance().createCard("面包房", this));
    cards.append(CardFactory::instance().createCard("林场", this));
    cards.append(CardFactory::instance().createCard("农场", this));
    cards.append(CardFactory::instance().createCard("金枪鱼船", this));
    cards.append(CardFactory::instance().createCard("寿司店",this));
    cards.append(CardFactory::instance().createCard("果蔬超市",this));
    cards.append(CardFactory::instance().createCard("会员俱乐部", this));
    cards.append(CardFactory::instance().createCard("商业中心",this));*/
    //初始地标建筑
    cards.append(CardFactory::instance().createCard("港口", this));
    cards.append(CardFactory::instance().createCard("火车站", this));
    cards.append(CardFactory::instance().createCard("购物中心", this));
    cards.append(CardFactory::instance().createCard("游乐园", this));
    cards.append(CardFactory::instance().createCard("广播塔", this));
    cards.append(CardFactory::instance().createCard("机场", this));
    //保存id映射
    for(Card* card:cards)
        m_allCardInstances[card->getId()]=card;

    return cards;
}

QList<Card*> GameState::getStoresInitCards(){
    // 商店临时牌堆
    QList<Card*> cards;

    int playerCount=m_players.size();

    // 蓝卡各六个
    cards.append(CardFactory::instance().createCards("麦田", 6, this));
    cards.append(CardFactory::instance().createCards("果园", 6, this));
    cards.append(CardFactory::instance().createCards("花田", 6, this)); // 注意：如果你的卡牌名称是“花田”，请改为“花田”
    cards.append(CardFactory::instance().createCards("林场", 6, this));
    cards.append(CardFactory::instance().createCards("鲭鱼船", 6, this));
    cards.append(CardFactory::instance().createCards("农场", 6, this));
    cards.append(CardFactory::instance().createCards("金枪鱼船", 6, this));

    // 绿卡各六个
    cards.append(CardFactory::instance().createCards("面包房", 6, this));
    cards.append(CardFactory::instance().createCards("奶酪工厂", 6, this));
    cards.append(CardFactory::instance().createCards("拆迁公司", 6, this));
    cards.append(CardFactory::instance().createCards("花店", 6, this));
    cards.append(CardFactory::instance().createCards("果蔬超市", 6, this)); // 注意：如果你的卡牌名称是“果树超市”，请改为“果树超市”
    cards.append(CardFactory::instance().createCards("家具工厂", 6, this));
    cards.append(CardFactory::instance().createCards("杂货店", 6, this));
    cards.append(CardFactory::instance().createCards("搬家公司", 6, this));
    cards.append(CardFactory::instance().createCards("饮料工厂", 6, this)); // 注意：如果你的卡牌名称是“饮料工厂”，请改为“饮料工厂”

    // 红卡各六个
    cards.append(CardFactory::instance().createCards("咖啡馆", 6, this));
    cards.append(CardFactory::instance().createCards("西餐厅", 6, this)); // 注意：如果你的卡牌名称是“西餐厅”，请改为“西餐厅”
    cards.append(CardFactory::instance().createCards("法国餐厅", 6, this));
    cards.append(CardFactory::instance().createCards("会员俱乐部", 6, this));
    cards.append(CardFactory::instance().createCards("披萨店", 6, this));
    cards.append(CardFactory::instance().createCards("寿司店", 6, this)); // 注意：如果你的卡牌名称是“寿司店”，请改为“寿司店”

    // 紫卡各（玩家个数）个
    cards.append(CardFactory::instance().createCards("商业中心", playerCount, this));
    cards.append(CardFactory::instance().createCards("出版社", playerCount, this));
    cards.append(CardFactory::instance().createCards("体育馆", playerCount, this));
    cards.append(CardFactory::instance().createCards("税务所", playerCount, this));
    cards.append(CardFactory::instance().createCards("科技公司", playerCount, this));

    //保存id映射
    for(Card* card:cards)
        m_allCardInstances[card->getId()]=card;

    return cards;
}

void GameState::initState() {
    // 获取玩家人数
    int playerCount = m_players.size();

    // 设置每个玩家的牌
    for(Player* player:m_players){
        QList<Card*>cards=getPlayerInitCards();
        for(Card* card:cards)
            player->addCard(card);
        //初始3元
        player->addCoins(3);
    }

    //设置每个商店的牌
    QList<Card*>cards=getStoresInitCards();

    //分类进商店
    for(Card* card:cards)
        if(card->getColor()==Color::Purple)//紫建筑店
            m_cardStores[2]->addCard(card);
        else if(card->getActLNum()>=7)//大数店
            m_cardStores[1]->addCard(card);
        else if(card->getActRNum()<=6)//小数店
            m_cardStores[0]->addCard(card);
        else
            qFatal("商店卡牌归类错误!");

    //洗牌
    for(CardStore* store:m_cardStores)
        store->shuffleCard();

    //补充卡槽
    for(CardStore* store:m_cardStores)
        store->suppleCard();

}

void GameState::addPlayer(Player* player) {
    m_players.append(player);
    m_allPlayerInstances[player->getId()]=player;
}


void GameState::nextPlayer() {
    //如果为空，则第一个玩家
    if(m_currentPlayer==nullptr){
        m_currentPlayer=m_players.first();
        return;
    }
    //计算当前玩家位置
    int currentIndex = m_players.indexOf(m_currentPlayer);
    //下一个玩家位置
    int nextIndex =(currentIndex+1)%m_players.size();
    //更换玩家
    m_currentPlayer=m_players.at(nextIndex);
    emit currentPlayerChanged(m_currentPlayer);
}

void GameState::addLog(QString log){
    if(log=="")
        return;
    m_log.append(log);
    emit logMessageAdded(log);
};


