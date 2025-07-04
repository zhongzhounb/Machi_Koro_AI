#ifndef CARDSTOREAREAWIDGET_H
#define CARDSTOREAREAWIDGET_H

#include <QWidget>
#include <QGridLayout> // 改变为 QGridLayout
#include <QList>
#include <QLabel>
#include <QMap> // 用于存储 CardStore 到 SlotWidget 列表的映射
#include <QQueue> // 用于动画队列
#include <QPropertyAnimation> // 用于动画

#include "gamestate.h" // 包含 CardStore, Card
// #include "cardstorewidget.h" // 不再需要 CardStoreWidget

class SlotWidget; // 前向声明
class Card;       // 前向声明
class CardWidget; // 前向声明
class CardStore;  // 前向声明

// 结构体，用于存储动画所需的数据 (从 CardStoreWidget 移动过来)
struct CardAnimationData {
    CardStore* store;               // 动画所属的 CardStore
    Card* cardData;                 // 正在移动的卡牌数据 (用于识别)
    int targetSlotPos;          // 目标槽位的位置 (这里是 CardStore 的 0-indexed 商店槽位索引)
    QPropertyAnimation* animation; // 动画对象
    CardWidget* animatedCardWidget;  // 用于动画的临时 CardWidget
    bool isRemovalAnimation; // 标记是否是移除动画
};

class CardStoreAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardStoreAreaWidget(QWidget* parent = nullptr);
    ~CardStoreAreaWidget();

    void setGameState(GameState* gameState);

public slots:
    // 从 CardStoreWidget 移动过来的槽函数
    void onSupplyCardAdded(CardStore* store);
    void onCardAdded(CardStore* store, Card* card, int pos);

private slots:
    // 从 CardStoreWidget 移动过来的槽函数
    void startNextAnimation();
    void animationFinished();
    void onSlotTopCardClicked(Card* clickedCard); // SlotWidget 不再传递索引，由 CardStoreAreaWidget 确定

private:
    GameState* m_gameState;
    QGridLayout* m_mainLayout; // 改变为 QGridLayout

    // 存储每个 CardStore 对应的 SlotWidget 列表
    QMap<CardStore*, QList<SlotWidget*>> m_storeToSlotsMap;

    // 动画相关成员 (从 CardStoreWidget 移动过来)
    QQueue<CardAnimationData> m_animationQueue;
    bool m_animationInProgress;

    void commonInit();
    void initializeStoreWidgets(); // 根据 GameState 初始化商店部件

    // 辅助函数：根据 Card 对象获取其图片路径 (如果需要，但动画直接使用 CardWidget)
    QString getCardImagePath(Card* card); // 从 CardStoreWidget 移动过来

    // 常量 (从 CardStoreWidget 移动过来)
    const QString SUPPLY_PILE_BACK_IMAGE_PATH = ":/resources/images/card/card_back.png";
};

#endif // CARDSTOREAREAWIDGET_H
