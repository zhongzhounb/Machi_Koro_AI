#include "cardstoreareawidget.h"
#include "slotwidget.h"
#include "card.h"
#include "cardwidget.h"
#include "cardstore.h"
#include "gamestate.h" // 确保包含 GameState 头文件
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QPoint>

CardStoreAreaWidget::CardStoreAreaWidget(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_mainLayout = new QGridLayout(this);
    setLayout(m_mainLayout);
}

CardStoreAreaWidget::~CardStoreAreaWidget() {}

void CardStoreAreaWidget::setGameState(GameState* gameState) {
    if (m_gameState == gameState) return;
    m_gameState = gameState;
    initializeStoreWidgets();
}

void CardStoreAreaWidget::initializeStoreWidgets()
{
    QLayoutItem *item;
    while ((item = m_mainLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
    m_storeToSlotsMap.clear();
    m_animationQueues.clear(); // 清除旧的动画队列
    m_animationInProgress.clear(); // 清除旧的动画状态

    if (!m_gameState) return;
    QList<CardStore*> stores = m_gameState->getCardStores();
    if (stores.isEmpty()) return;

    int maxSlotsInAnyStore = 0;
    for (CardStore* store : stores) {
        maxSlotsInAnyStore = qMax(maxSlotsInAnyStore, store->getStoreSlotsCount() + 1);
        m_animationInProgress.insert(store, false); // 初始化每个store的动画状态为false
    }

    int row_index = 0;
    for (CardStore* store : stores) {
        QList<SlotWidget*> currentStoreSlots;

        SlotWidget* supplySlot = new SlotWidget(true, store->getType(), this);
        m_mainLayout->addWidget(supplySlot, row_index, 0);
        currentStoreSlots.append(supplySlot);
        connect(store, &CardStore::supplyCardAdded, this, &CardStoreAreaWidget::onSupplyCardAdded);
        connect(store,&CardStore::cardDeled,this,&CardStoreAreaWidget::onCardDeled);

        for (int i = 0; i < store->getStoreSlotsCount(); i++) {
            SlotWidget* slot = new SlotWidget(false, Color::BackNone, this);
            QList<Card*> cards = store->getSlots().at(i);
            for (Card* card : cards) {
                CardWidget* cardWidget = new CardWidget(card, ShowType::Ordinary, slot);
                slot->pushCard(cardWidget);
            }
            m_mainLayout->addWidget(slot, row_index, i + 1);
            currentStoreSlots.append(slot);
        }

        m_storeToSlotsMap.insert(store, currentStoreSlots);
        connect(store, &CardStore::cardAdded, this, &CardStoreAreaWidget::onCardAdded);
        row_index++;
    }

    for(int i = 0; i < maxSlotsInAnyStore; i++)
        m_mainLayout->setColumnStretch(i, 1);
    for(int i = 0; i < stores.size(); i++)
        m_mainLayout->setRowStretch(i, 1);
}

QPoint CardStoreAreaWidget::getStoreSlotCenterPos(CardStore* store, int slotIndexInStore) {
    if (!m_storeToSlotsMap.contains(store)) {
        qWarning() << "CardStoreAreaWidget: Store not found in map.";
        return QPoint();
    }

    // slotIndexInStore 是 CardStore::getSlots() 中的索引 (0-based)
    // 而 m_storeToSlotsMap 中的 QList<SlotWidget*> 包含 supplySlot 在索引 0
    // 所以实际卡槽的索引是 slotIndexInStore + 1
    int actualSlotWidgetIndex = slotIndexInStore + 1;

    QList<SlotWidget*> storeSlots = m_storeToSlotsMap.value(store);
    if (actualSlotWidgetIndex >= storeSlots.size() || actualSlotWidgetIndex < 1) { // 确保是有效的卡槽索引
        qWarning() << "CardStoreAreaWidget: Invalid slot index in store for position lookup.";
        return QPoint();
    }

    SlotWidget* slot = storeSlots[actualSlotWidgetIndex];
    if (!slot) return QPoint();

    // 返回 SlotWidget 在 CardStoreAreaWidget 局部坐标系中的中心点
    return slot->mapToParent(slot->rect().center());
}

// 新增函数：处理下一个动画任务
void CardStoreAreaWidget::processNextAnimation(CardStore* store)
{
    // 如果队列为空，则表示该store没有待处理的动画，将动画状态设为false
    if (m_animationQueues[store].isEmpty()) {
        m_animationInProgress[store] = false;
        return;
    }

    // 从队列中取出第一个任务
    QPair<Card*, int> task = m_animationQueues[store].takeFirst();
    Card* card = task.first;
    int pos = task.second;

    m_animationInProgress[store] = true; // 标记该store正在进行动画

    int slot_index = pos + 1; // 偏移量，因为 storeSlots[0] 是供应堆
    QList<SlotWidget*> storeSlots = m_storeToSlotsMap.value(store);
    if (slot_index >= storeSlots.size()) {
        qWarning() << "CardStoreAreaWidget: Invalid slot index for animation. Skipping task.";
        processNextAnimation(store); // 尝试处理下一个任务
        return;
    }

    SlotWidget* supplySlot = storeSlots[0];
    SlotWidget* targetSlot = storeSlots[slot_index];

    // 1. 从供应堆中移除一张卡片（更新计数或视觉效果）
    supplySlot->popCard();

    // 2. 创建一个用于动画的临时 CardWidget
    CardWidget* animatingCardWidget = new CardWidget(card, ShowType::Ordinary, this);
    animatingCardWidget->setFixedSize(targetSlot->size());
    animatingCardWidget->show();
    animatingCardWidget->raise();

    QPoint startGlobalPos = supplySlot->mapToGlobal(QPoint(0, 0));
    QPoint endGlobalPos = targetSlot->mapToGlobal(QPoint(0, 0));

    QPoint startLocalPos = this->mapFromGlobal(startGlobalPos);
    QPoint endLocalPos = this->mapFromGlobal(endGlobalPos);

    animatingCardWidget->move(startLocalPos);

    // 步骤1: 延迟0.5秒后开始移动动画
    QTimer::singleShot(500, this, [this, animatingCardWidget, card, targetSlot, startLocalPos, endLocalPos, store]() {
        QPropertyAnimation* animation = new QPropertyAnimation(animatingCardWidget, "pos", this);
        animation->setDuration(500); // 动画持续时间 500 毫秒
        animation->setStartValue(startLocalPos);
        animation->setEndValue(endLocalPos);
        animation->setEasingCurve(QEasingCurve::OutQuad); // 使用缓出四次方曲线，使动画更平滑自然

        // 步骤2: 连接动画完成信号，并在动画停止后再次延迟0.5秒
        connect(animation, &QPropertyAnimation::finished, this, [this, animatingCardWidget, card, targetSlot, store]() {
            // 动画到达目标位置后，再等待0.5秒
            QTimer::singleShot(500, this, [this, animatingCardWidget, card, targetSlot, store]() {
                // 删除用于动画的临时 CardWidget
                animatingCardWidget->deleteLater();

                // 创建实际的 CardWidget 并将其推入目标卡槽
                CardWidget* newCardWidget = new CardWidget(card, ShowType::Ordinary, targetSlot);
                targetSlot->pushCard(newCardWidget);

                this->updateGeometry();

                // 当前动画完全结束后，处理该store的下一个动画任务
                processNextAnimation(store);
            });
        });

        animation->start(QAbstractAnimation::DeleteWhenStopped);
    });
}


void CardStoreAreaWidget::onCardAdded(CardStore* store, Card* card, int pos)
{
    if (!m_storeToSlotsMap.contains(store)) return;

    // 将新的动画任务添加到对应store的队列中
    m_animationQueues[store].append({card, pos});

    // 如果该store当前没有动画正在进行，则立即开始处理队列中的第一个任务
    if (!m_animationInProgress.value(store, false)) {
        processNextAnimation(store);
    }
}

void CardStoreAreaWidget::onSupplyCardAdded(CardStore* store){
    if (!m_storeToSlotsMap.contains(store)) return;
    m_storeToSlotsMap.value(store).at(0)->addCount();
}

void CardStoreAreaWidget::onCardDeled(CardStore* store,Card* card,int pos){
    if (!m_storeToSlotsMap.contains(store)) return;

    pos++; // 偏移量，因为m_slots[0]是供应堆
    QList<SlotWidget*> storeSlots = m_storeToSlotsMap.value(store);
    if (pos >= storeSlots.size()) return;

    storeSlots[pos]->popCard();
}
