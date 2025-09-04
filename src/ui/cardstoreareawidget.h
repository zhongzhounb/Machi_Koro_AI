#ifndef CARDSTOREAREAWIDGET_H
#define CARDSTOREAREAWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QMap>
#include <QList>
#include <QPair>
#include <QPoint> // 新增：QPoint 头文件

// 前向声明
class GameState;
class CardStore;
class SlotWidget;
class Card;
class CardWidget;

class CardStoreAreaWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CardStoreAreaWidget(QWidget* parent = nullptr);
    ~CardStoreAreaWidget();

    void setGameState(GameState* gameState);

    // 新增：获取商店中某个卡槽的中心位置（在 CardStoreAreaWidget 的局部坐标系中）
    // slotIndexInStore 是卡牌在 CardStore::getSlots() 列表中的索引
    QPoint getStoreSlotCenterPos(CardStore* store, int slotIndexInStore);

    // 暴露 m_storeToSlotsMap 给 MainWindow 用于获取 SlotWidget 的尺寸
    QMap<CardStore*, QList<SlotWidget*>> m_storeToSlotsMap; // 注意：如果希望保持私有，可以添加一个公共方法来获取 SlotWidget*

signals:
    // 新增：转发 CardWidget 的请求显示详细卡牌信号
    void cardWidgetRequestShowDetailed(Card* card, QPoint globalPos);
    // 新增：转发 CardWidget 的请求隐藏详细卡牌信号
    void cardWidgetRequestHideDetailed();

private slots:
    void onCardAdded(CardStore* store, Card* card, int pos);
    void onSupplyCardAdded(CardStore* store);
    void onCardDeled(CardStore* store, Card* card, int pos);
    // 新增：处理内部 SlotWidget 的请求
    void handleSlotWidgetRequestShowDetailed(Card* card, QPoint globalPos);
    void handleSlotWidgetRequestHideDetailed();

private:
    void initializeStoreWidgets();
    void processNextAnimation(CardStore* store);

    GameState* m_gameState = nullptr;
    QGridLayout* m_mainLayout = nullptr;
    // QMap<CardStore*, QList<SlotWidget*>> m_storeToSlotsMap; // 移到 public

    QMap<CardStore*, QList<QPair<Card*, int>>> m_animationQueues;
    QMap<CardStore*, bool> m_animationInProgress;
};

#endif // CARDSTOREAREAWIDGET_H
