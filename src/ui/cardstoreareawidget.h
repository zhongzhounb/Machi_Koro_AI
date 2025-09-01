#ifndef CARDSTOREAREAWIDGET_H
#define CARDSTOREAREAWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QMap>
#include <QList>
#include <QPair> // 用于存储卡片和位置的对

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

private slots:
    void onCardAdded(CardStore* store, Card* card, int pos);
    void onSupplyCardAdded(CardStore* store);
    void onCardDeled(CardStore* store, Card* card, int pos);

private:
    void initializeStoreWidgets();
    void processNextAnimation(CardStore* store); // 新增：处理下一个动画任务

    GameState* m_gameState = nullptr;
    QGridLayout* m_mainLayout = nullptr;
    QMap<CardStore*, QList<SlotWidget*>> m_storeToSlotsMap;

    // 新增：动画队列和动画状态
    QMap<CardStore*, QList<QPair<Card*, int>>> m_animationQueues; // 每个CardStore的动画任务队列
    QMap<CardStore*, bool> m_animationInProgress; // 标记每个CardStore是否有动画正在进行
};

#endif // CARDSTOREAREAWIDGET_H
