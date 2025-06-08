#ifndef CARDSTORE_H
#define CARDSTORE_H

#include"global.h"

class Card;
class Player;

class CardStore : public QObject
{
    Q_OBJECT
public:
    // 构造函数：接收所有卡牌模板的映射和随机数生成器
    explicit CardStore(const QList<Card*>& allCards, QRandomGenerator* randomGenerator, QObject* parent = nullptr);
    ~CardStore();

    // 初始化商店显示
    void initializeShopDisplay();
    // 补充商店 (在购买后调用)
    void replenishShopDisplay();
    // 从商店显示中移除一张卡牌模板 (购买时调用)
    void removeCardFromDisplay(Card* cardTemplate);

    // 获取所有可供购买的卡牌模板 (UI/AI用于展示市场)
    QList<Card*> getMarketCards(Player* player) const; // 传入 player 以便判断地标是否已建造

signals:
    // 商店显示更新信号 (可选，用于UI刷新)
    void shopDisplayChanged();

private:
    const QMap<int, Card*>& m_allCardTemplates; // 对所有卡牌模板的引用 (Game 拥有它们)
    QRandomGenerator* m_randomGenerator;        // 对 Game 的随机数生成器的引用

    // 商店显示列表 (存储 Card* 模板)
    QList<Card*> m_small;       // 1-6点激活的红/蓝/绿牌 (最多5种)
    QList<Card*> m_big;    // 7+点激活的红/蓝/绿牌 (最多5种)
    QList<Card*> m_purple;    // 固定2种紫卡
    QList<Card*> m_landmarks; // 所有地标卡牌

    // 卡堆（用于补充商店的卡牌模板）
    QList<Card*> m_small_pool;    // 所有1-6点激活的红/蓝/绿牌
    QList<Card*> m_big_pool; // 所有7+点激活的红/蓝/绿牌
    QList<Card*> m__purple_pool; // 所有非固定紫卡

    // 辅助函数：填充卡牌池 (在构造函数中调用一次)
    void populateCardPools();
    // 辅助函数：补充商店的某个区域
    void replenishShopSection(QList<Card*>& shopSection, QList<Card*>& sourcePool, int targetSize);
};

#endif // CARDSTORE_H
