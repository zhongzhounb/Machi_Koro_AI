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
                // !!! 修改点：
                // 将 CardWidget 的父对象设置为它所在的 slot，这更符合逻辑
                CardWidget* cardWidget = new CardWidget(card, ShowType::Ordinary, slot);
                slot->pushCard(cardWidget);
            }
            m_mainLayout->addWidget(slot, row_index, i + 1);
            currentStoreSlots.append(slot);
        }

        // 将当前 CardStore 及其对应的 SlotWidget 列表存入映射
        m_storeToSlotsMap.insert(store, currentStoreSlots);

        // 连接 CardStore 的卡牌添加信号到 CardStoreAreaWidget 的槽
        connect(store, &CardStore::cardAdded, this, &CardStoreAreaWidget::onCardAdded);

        row_index++;

    }

    QTimer::singleShot(0, this, &CardStoreAreaWidget::updateAllCardSizes);

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
    CardWidget* newCardWidget = new CardWidget(card, ShowType::Ordinary, storeSlots[pos]);
    storeSlots[pos]->pushCard(newCardWidget);

    // !!! 新增：
    // 添加新卡后，立即更新其尺寸以匹配现有卡牌
    updateAllCardSizes();

}


void CardStoreAreaWidget::onSupplyCardAdded(CardStore* store){
    if (!m_storeToSlotsMap.contains(store)) {
        qWarning() << "CardStoreAreaWidget: Received supplyCardAdded for unknown store.";
        return;
    }
    m_storeToSlotsMap.value(store).at(0)->addCount();
}

void CardStoreAreaWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); // 必须先调用基类的实现
    updateAllCardSizes();        // 然后调用我们的尺寸更新函数
}

void CardStoreAreaWidget::updateAllCardSizes()
{
    if (!m_mainLayout || m_mainLayout->rowCount() == 0 || m_mainLayout->columnCount() == 0) {
        return; // 如果布局未就绪，则不执行任何操作
    }

    // 1. 获取布局的可用几何区域
    QRect layoutRect = m_mainLayout->contentsRect();
    int availableWidth = layoutRect.width();
    int availableHeight = layoutRect.height();

    // 2. 计算单个单元格的平均尺寸 (减去间距)
    availableWidth -= m_mainLayout->horizontalSpacing() * (m_mainLayout->columnCount() - 1);
    availableHeight -= m_mainLayout->verticalSpacing() * (m_mainLayout->rowCount() - 1);

    int cellWidth = availableWidth / m_mainLayout->columnCount();
    int cellHeight = availableHeight / m_mainLayout->rowCount();

    // 3. 为了保持宽高比（我们假设为1:1），卡牌的尺寸必须是单元格宽度和高度中的较小者
    int cardSize = qMin(cellWidth, cellHeight);

    if (cardSize <= 0) {
        return; // 避免无效尺寸
    }

    // 4. 遍历所有 SlotWidget，并命令它们更新其内部卡牌的尺寸
    // 使用 C++11 的 range-based for loop 遍历 map 的值
    for (const QList<SlotWidget*>& slotList : m_storeToSlotsMap) {
        for (SlotWidget* slot : slotList) {
            if (slot) {
                // 调用我们在 SlotWidget 中添加的函数
                slot->updateCardSize(cardSize);
            }
        }
    }
}
