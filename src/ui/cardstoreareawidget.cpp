#include "cardstoreareawidget.h"
#include <QDebug>
#include <QLabel>
#include <QTimer> // For animation
#include <QGraphicsOpacityEffect> // For potential fade effects (if needed by animation)

// 引入 SlotWidget, Card, CardWidget 的定义
#include "slotwidget.h"
#include "card.h"
#include "cardwidget.h"
#include "cardstore.h"
#include "aspectratiowidget.h"


// 新增构造函数：供 Qt Designer/uic 使用
CardStoreAreaWidget::CardStoreAreaWidget(QWidget* parent)
    : QWidget(parent)
{
    m_mainLayout = new QGridLayout(this);
    setLayout(m_mainLayout); // 设置主布局
    // GameState 将通过 setGameState() 方法稍后设置
}

CardStoreAreaWidget::~CardStoreAreaWidget()
{

}

// 新增方法：设置 GameState 并初始化商店部件
void CardStoreAreaWidget::setGameState(GameState* gameState) {
    if (m_gameState == gameState) return; // 避免重复设置
    m_gameState = gameState;
    initializeStoreWidgets(); // 现在 GameState 已设置，可以初始化 UI 了
}

void CardStoreAreaWidget::initializeStoreWidgets()
{
    // 清除 QGridLayout 中的所有项
    QLayoutItem *item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater(); // 安全删除部件
        }
        delete item; // 删除布局项
    }

    QList<CardStore*> stores = m_gameState->getCardStores();
    if (stores.isEmpty()) {
        qDebug() << "No card stores found in GameState.";
        QLabel* noStoresLabel = new QLabel("No card stores available.", this);
        noStoresLabel->setAlignment(Qt::AlignCenter);
        m_mainLayout->addWidget(noStoresLabel, 0, 0, 1, -1); // 占据第一行所有列
        return;
    }

    // 确定最大列数，以便设置 QGridLayout 的列伸缩因子
    int maxSlotsInAnyStore = 0;
    for (CardStore* store : stores) {
        // +1 for the supply pile slot
        maxSlotsInAnyStore = qMax(maxSlotsInAnyStore, store->getStoreSlotsCount() + 1);
    }

    int row_index = 0;
    for (CardStore* store : stores) {
        QList<SlotWidget*> currentStoreSlots; // 存储当前 CardStore 的 SlotWidget 列表

        // 第一个槽位是供应堆
        SlotWidget* supplySlot = new SlotWidget(true, store->getType(), this);
        m_mainLayout->addWidget(supplySlot, row_index, 0); // 添加到网格的 (row_index, 0)
        currentStoreSlots.append(supplySlot);
        // 连接 CardStore 的供应堆信号到 CardStoreAreaWidget 的槽
        connect(store, &CardStore::supplyCardAdded, this, &CardStoreAreaWidget::onSupplyCardAdded);


        // 剩余槽位是普通商店槽位
        for (int i = 0; i < store->getStoreSlotsCount(); i++) {
            SlotWidget* slot = new SlotWidget(false, Color::BackNone, this);
            QList<Card*> cards = store->getSlots().at(i);
            for (Card* card : cards) {
                // CardWidget 的父对象设置为 CardStoreAreaWidget
                slot->pushCard(new CardWidget(card,ShowType::Ordinary, this));
            }
            m_mainLayout->addWidget(slot, row_index, i + 1); // 添加到网格的 (row_index, i+1)
            currentStoreSlots.append(slot);
        }

        // 将当前 CardStore 及其对应的 SlotWidget 列表存入映射
        m_storeToSlotsMap.insert(store, currentStoreSlots);

        // 连接 CardStore 的卡牌添加信号到 CardStoreAreaWidget 的槽
        connect(store, &CardStore::cardAdded, this, &CardStoreAreaWidget::onCardAdded);

        row_index++;
    }

    for(int i=0;i<maxSlotsInAnyStore;i++)
        m_mainLayout->setColumnStretch(i,1);
    for(int i=0;i<stores.size();i++)
        m_mainLayout->setRowStretch(i,1);

}


void CardStoreAreaWidget::onCardAdded(CardStore* store, Card* card, int pos)
{
    // 确保 store 存在于映射中
    if (!m_storeToSlotsMap.contains(store)) {
        qWarning() << "CardStoreAreaWidget: Received cardAdded for unknown store.";
        return;
    }

    //位移，因为m_slots[0]是供应堆
    pos++;

    QList<SlotWidget*> storeSlots = m_storeToSlotsMap.value(store);
    if (pos >= storeSlots.size()) {
        qWarning() << "CardStoreAreaWidget: Target slot position out of bounds for store:" << "pos:" << pos;
        return;
    }

    storeSlots[pos]->pushCard(new CardWidget(card,ShowType::Ordinary,this));

}


void CardStoreAreaWidget::onSupplyCardAdded(CardStore* store){
    if (!m_storeToSlotsMap.contains(store)) {
        qWarning() << "CardStoreAreaWidget: Received supplyCardAdded for unknown store.";
        return;
    }
    m_storeToSlotsMap.value(store).at(0)->addCount();
}
