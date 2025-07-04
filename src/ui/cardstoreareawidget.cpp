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


// 辅助函数：根据 Card 对象获取其图片路径
QString CardStoreAreaWidget::getCardImagePath(Card* card) {
    if (!card) return "";
    // 假设 CardWidget 的 classNameToImagePath 逻辑在此处也适用
    // 注意：这个函数在当前实现中可能没有被直接使用，因为动画直接创建 CardWidget
    return QString(":/resources/images/card/img/%1.png").arg(card->metaObject()->className());
}

// 提取公共初始化逻辑
void CardStoreAreaWidget::commonInit() {
    m_mainLayout = new QGridLayout(this); // 改变为 QGridLayout
    m_gameState = nullptr; // 初始时将 GameState 设置为 nullptr
    m_animationInProgress = false; // 初始化动画状态
}

// 新增构造函数：供 Qt Designer/uic 使用
CardStoreAreaWidget::CardStoreAreaWidget(QWidget* parent)
    : QWidget(parent)
{
    commonInit(); // 调用公共初始化
    setLayout(m_mainLayout); // 设置主布局
    // GameState 将通过 setGameState() 方法稍后设置
}

CardStoreAreaWidget::~CardStoreAreaWidget()
{
    // 清理任何待处理的动画及其关联的 CardWidget
    while (!m_animationQueue.isEmpty()) {
        CardAnimationData data = m_animationQueue.dequeue();
        if (data.animation) {
            data.animation->stop();
            data.animation->deleteLater(); // 安排删除
        }
        if (data.animatedCardWidget) {
            // 如果 animatedCardWidget 还没有被重新父化到 SlotWidget，则删除它
            // 如果已经被重新父化，则由 SlotWidget 或其父级负责删除
            if (data.animatedCardWidget->parent() == this) { // 检查父对象是否是 CardStoreAreaWidget
                data.animatedCardWidget->deleteLater(); // 安排删除
            }
        }
    }
    // Qt 的父子机制会处理 m_mainLayout 和其内容的删除。
}

// 新增方法：设置 GameState 并初始化商店部件
void CardStoreAreaWidget::setGameState(GameState* gameState) {
    if (m_gameState == gameState) return; // 避免重复设置
    m_gameState = gameState;
    initializeStoreWidgets(); // 现在 GameState 已设置，可以初始化 UI 了
}

void CardStoreAreaWidget::initializeStoreWidgets()
{
    if (!m_gameState) {
        qWarning() << "CardStoreAreaWidget: GameState is null. Cannot initialize store widgets.";
        // 清除现有布局内容，并显示一个提示
        QLayoutItem *item;
        while ((item = m_mainLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        QLabel* noStateLabel = new QLabel("No GameState provided yet.", this);
        noStateLabel->setAlignment(Qt::AlignCenter);
        m_mainLayout->addWidget(noStateLabel, 0, 0, 1, -1); // 占据第一行所有列
        // m_mainLayout->addStretch(); // QGridLayout 通常不需要 addStretch 来推内容
        return;
    }

    // 清除任何现有的部件和布局项
    // 遍历并删除所有 SlotWidget
    for (auto it = m_storeToSlotsMap.begin(); it != m_storeToSlotsMap.end(); ++it) {
        qDeleteAll(it.value());
    }
    m_storeToSlotsMap.clear();

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
            // 连接 SlotWidget 的点击信号到 CardStoreAreaWidget 的槽
            connect(slot, &SlotWidget::topCardClickedInSlot, this, &CardStoreAreaWidget::onSlotTopCardClicked);
        }

        // 将当前 CardStore 及其对应的 SlotWidget 列表存入映射
        m_storeToSlotsMap.insert(store, currentStoreSlots);

        // 连接 CardStore 的卡牌添加信号到 CardStoreAreaWidget 的槽
        connect(store, &CardStore::cardAdded, this, &CardStoreAreaWidget::onCardAdded);

        row_index++;
    }

    // 设置 QGridLayout 的列伸缩因子，确保所有列等宽
    for (int i = 0; i < maxSlotsInAnyStore; ++i) {
        m_mainLayout->setColumnStretch(i, 1);
    }
    // 设置行伸缩因子，确保所有行等高
    for (int i = 0; i < stores.size(); ++i) {
        m_mainLayout->setRowStretch(i, 1);
    }
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

    // 获取供应堆和目标槽位的绝对位置（相对于 CardStoreAreaWidget）
    QPoint supplyPos = storeSlots[0]->mapToParent(QPoint(0, 0));
    QPoint targetPos = storeSlots[pos]->mapToParent(QPoint(0, 0));

    // 如果仍然为 (0, 0)，尝试 mapToGlobal 并转换坐标系
    if (supplyPos == QPoint(0, 0)) {
        supplyPos = mapFromGlobal(storeSlots[0]->mapToGlobal(QPoint(0, 0)));
        targetPos = mapFromGlobal(storeSlots[pos]->mapToGlobal(QPoint(0, 0)));
    }

    // 设置动画的起始和终止位置
    QRect supplyRect(supplyPos, storeSlots[0]->size());
    QRect targetRect(targetPos, storeSlots[pos]->size());
    // 1. 创建一个用于动画的临时 CardWidget，父对象设为 CardStoreAreaWidget
    CardWidget* animatedCard = new CardWidget(card,ShowType::Ordinary, this);
    // 设置其初始位置为供应堆的几何形状（相对于 CardStoreAreaWidget 坐标系）

    animatedCard->setGeometry(supplyRect);
    animatedCard->show();
    animatedCard->raise(); // 确保动画卡牌在最上层

    // 3. 创建动画
    QPropertyAnimation* animation = new QPropertyAnimation(animatedCard, "geometry", this);
    animation->setDuration(500); // 0.5 秒
    animation->setStartValue(animatedCard->geometry());
    animation->setEndValue(targetRect);
    animation->setEasingCurve(QEasingCurve::OutQuad); // 缓出曲线

    // 4. 将动画数据加入队列
    CardAnimationData data;
    data.store = store; // 存储所属的 CardStore
    data.cardData = card;
    data.targetSlotPos = pos;
    data.animation = animation;
    data.animatedCardWidget = animatedCard;
    data.isRemovalAnimation = false; // 这是一个添加动画
    m_animationQueue.enqueue(data);

    // 连接动画完成信号
    connect(animation, &QPropertyAnimation::finished, this, &CardStoreAreaWidget::animationFinished);

    // 如果没有其他动画正在进行，则启动下一个动画
    if (!m_animationInProgress) {
        startNextAnimation();
    }
    qDebug() << "Card added to store slot:" << card->getName() << "at pos:" << pos;

}


void CardStoreAreaWidget::startNextAnimation()
{
    if (m_animationQueue.isEmpty()) {
        m_animationInProgress = false;
        return;
    }

    m_animationInProgress = true;
    CardAnimationData data = m_animationQueue.head(); // 获取队列头部数据，但不移除
    data.animation->start();
}

void CardStoreAreaWidget::animationFinished()
{
    QPropertyAnimation* senderAnimation = qobject_cast<QPropertyAnimation*>(sender());
    if (!senderAnimation) return;

    // 查找队列中的动画数据
    int index = -1;
    for (int i = 0; i < m_animationQueue.size(); ++i) {
        if (m_animationQueue.at(i).animation == senderAnimation) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        qWarning() << "Animation finished but not found in queue. Cleaning up anyway.";
        senderAnimation->deleteLater();
        return;
    }

    CardAnimationData data = m_animationQueue.dequeue(); // 从队列中移除数据

    if (!data.isRemovalAnimation) { // 卡牌添加动画完成
        // 将动画用的 CardWidget 重新父化到目标 SlotWidget
        // 确保 store 存在于映射中
        if (!m_storeToSlotsMap.contains(data.store)) {
            qWarning() << "CardStoreAreaWidget: Animation finished for unknown store. Cannot re-parent CardWidget.";
            data.animatedCardWidget->deleteLater(); // 无法找到目标槽，直接删除动画卡牌
        } else {
            QList<SlotWidget*> storeSlots = m_storeToSlotsMap.value(data.store);
            if (data.targetSlotPos >= storeSlots.size()) {
                qWarning() << "CardStoreAreaWidget: Animation finished, target slot pos out of bounds. Cannot re-parent CardWidget.";
                data.animatedCardWidget->deleteLater(); // 目标槽位无效，直接删除动画卡牌
            } else {
                SlotWidget* targetSlot = storeSlots[data.targetSlotPos];
                // 在推入之前隐藏，SlotWidget 会负责显示
                data.animatedCardWidget->hide();
                targetSlot->pushCard(data.animatedCardWidget);
                // SlotWidget::pushCard 内部已经设置了父对象，这里可以省略
                // data.animatedCardWidget->setParent(targetSlot);
                qDebug() << "Card added animation finished for:" << data.cardData->getName() << "to slot:" << data.targetSlotPos;
            }
        }
    } else { // 卡牌移除动画完成 (尽管目前移除槽函数被注释，但动画数据结构中保留了此标记)
        // 删除动画用的 CardWidget，因为它已经离开场景
        data.animatedCardWidget->deleteLater();
        qDebug() << "Card removed animation finished for:" << data.cardData->getName() << "from slot:" << data.targetSlotPos;
    }

    // 清理动画对象
    data.animation->deleteLater();

    // 启动队列中的下一个动画
    startNextAnimation();
}

void CardStoreAreaWidget::onSlotTopCardClicked(Card* clickedCard)
{
    // 确定是哪个 SlotWidget 发出了信号
    SlotWidget* clickedSlot = qobject_cast<SlotWidget*>(sender());
    if (!clickedSlot) return;

    // 查找是哪个 CardStore 的哪个槽位被点击了
    CardStore* clickedStore = nullptr;
    int slotIndex = -1;

    for (auto it = m_storeToSlotsMap.begin(); it != m_storeToSlotsMap.end(); ++it) {
        int currentSlotIndex = it.value().indexOf(clickedSlot);
        if (currentSlotIndex != -1) {
            clickedStore = it.key();
            slotIndex = currentSlotIndex;
            break;
        }
    }

    if (clickedStore && slotIndex != -1) {
        qDebug() << "Card clicked in Store:"
        << "Slot Index:" << (slotIndex == 0 ? "Supply Pile" : QString::number(slotIndex -1)) // 调整显示索引
        << "Card Name:" << clickedCard->getName();
        // 在这里，您通常会向更高级别的控制器（例如 GameController）
        // 发出信号，以处理购买逻辑。
        // 示例：emit cardPurchased(clickedStore, clickedCard, slotIndex);
    } else {
        qWarning() << "CardStoreAreaWidget: Clicked slot not found in map.";
    }
}

void CardStoreAreaWidget::onSupplyCardAdded(CardStore* store){
    if (!m_storeToSlotsMap.contains(store)) {
        qWarning() << "CardStoreAreaWidget: Received supplyCardAdded for unknown store.";
        return;
    }
    m_storeToSlotsMap.value(store).at(0)->addCount();
}
