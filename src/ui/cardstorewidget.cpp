#include "cardstorewidget.h"
#include <QDebug>
#include <QTimer>
#include <QGraphicsOpacityEffect> // For potential fade effects

// 辅助函数：根据 Card 对象获取其图片路径（如果需要，但动画直接使用 CardWidget）
QString CardStoreWidget::getCardImagePath(Card* card) {
    if (!card) return "";
    // 假设 CardWidget 的 classNameToImagePath 逻辑在此处也适用
    return QString(":/resources/images/card/img/%1.png").arg(card->metaObject()->className());
}

CardStoreWidget::CardStoreWidget(CardStore* store, QWidget* parent)
    : QWidget(parent)
    , m_store(store)
    , m_mainLayout(new QHBoxLayout(this))
    , m_animationInProgress(false)
{
    initUI();

    // 连接 CardStore 的信号到 CardStoreWidget 的槽
    if (m_store) {
        connect(m_store, &CardStore::cardAdded, this, &CardStoreWidget::onCardAdded);
        connect(m_store, &CardStore::cardRemoved, this, &CardStoreWidget::onCardRemoved);
        connect(m_store, &CardStore::supplyCountChanged, this, &CardStoreWidget::onSupplyCountChanged);
    } else {
        qWarning() << "CardStoreWidget: CardStore pointer is null!";
    }

    setLayout(m_mainLayout);
}

CardStoreWidget::~CardStoreWidget()
{
    // 清理任何待处理的动画及其关联的 CardWidget
    while (!m_animationQueue.isEmpty()) {
        CardAnimationData data = m_animationQueue.dequeue();
        if (data.animation) {
            data.animation->stop();
            data.animation->deleteLater(); // 安排删除
        }
        if (data.animatedCardWidget) {
            data.animatedCardWidget->deleteLater(); // 安排删除
        }
    }
    // Qt 的父子机制会处理 m_mainLayout 和 m_slots 内容的删除。
}

void CardStoreWidget::initUI()
{
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    // 根据 CardStore 中的槽位数量创建 SlotWidget
    // 假设 m_store->getStoreSlotsCount() 返回总槽位数量 (1个供应堆 + N个商店槽位)
    int totalSlots = m_store ? m_store->getStoreSlotsCount() : 5; // 如果 store 为空，默认 5 个槽位

    for (int i = 0; i < totalSlots; ++i) {
        SlotWidget* slot;
        if (i == 0) { // 第一个槽位是供应堆
            slot = new SlotWidget(true, SUPPLY_PILE_BACK_IMAGE_PATH, this);
            // 初始化供应堆数量
            if (m_store) {
                slot->setDisplayedCount(m_store->getSupplyCount());
            }
            // 供应堆通常只显示一个卡牌背面，但其数量由 overlay 管理
            // 如果供应堆初始有卡，需要推入一个 CardWidget 来显示背面
            if (m_store && m_store->getSupplyCount() > 0) {
                // 创建一个代表卡牌背面的CardWidget，可以传入一个默认的Card或实际的Card
                // 这里我们传入一个模拟卡牌，CardWidget会根据它的类型/颜色显示背面
                Card* dummyCard = new Card("Dummy Back", 0, 0, 0, "", Color::None, CardType::Building, this); // 传入this作为父对象，方便管理
                slot->pushCard(new CardWidget(dummyCard, slot));
            }
        } else { // 剩余槽位是普通商店槽位
            slot = new SlotWidget(false, "", this);
        }
        m_slots.append(slot);
        m_mainLayout->addWidget(slot);

        // 连接 SlotWidget 的点击信号到 CardStoreWidget 的槽
        connect(slot, &SlotWidget::topCardClickedInSlot, this, &CardStoreWidget::onSlotTopCardClicked);
    }
}

void CardStoreWidget::updatePosition()
{
    // 对于 QHBoxLayout，通常不需要显式调用此函数，布局管理器会处理位置。
    // 如果有自定义的叠加元素或需要微调，可以在这里实现。
}

void CardStoreWidget::onCardAdded(CardStore* store, Card* card, int pos)
{
    if (!m_store || store != m_store) return;

    if (pos == 0) { // 卡牌添加到供应堆 (例如，新卡牌类型可用)
        SlotWidget* supplySlot = m_slots[0];
        // 如果供应堆之前是空的，现在有卡了，需要显示一张卡牌背面
        if (supplySlot->isEmpty() && m_store->getSupplyCount() > 0) {
            Card* dummyCard = new Card("Dummy Back", 0, 0, 0, "", Color::None, CardType::Building, this);
            supplySlot->pushCard(new CardWidget(dummyCard, supplySlot));
        }
        // 更新供应堆的数量显示
        supplySlot->setDisplayedCount(m_store->getSupplyCount());
        qDebug() << "Card added to supply pile (data only):" << card->getName() << "New count:" << m_store->getSupplyCount();

    } else if (pos >= 1 && pos < m_slots.size()) { // 卡牌添加到普通商店槽位
        SlotWidget* targetSlot = m_slots[pos];
        SlotWidget* supplySlot = m_slots[0];

        // 1. 创建一个用于动画的临时 CardWidget，父对象设为 CardStoreWidget
        CardWidget* animatedCard = new CardWidget(card, this);
        // 设置其初始位置为供应堆的几何形状（映射到 CardStoreWidget 坐标系）
        // 假设供应堆的 topCard() 返回的是其显示的卡牌，我们从那里开始动画
        if (supplySlot->topCard()) {
            animatedCard->setGeometry(supplySlot->mapTo(this, supplySlot->topCard()->geometry()));
        } else {
            // 如果供应堆没有 topCard (例如刚初始化或空)，则从供应槽位顶部开始
            animatedCard->setGeometry(supplySlot->mapTo(this, supplySlot->rect()));
        }
        animatedCard->show();
        animatedCard->raise(); // 确保动画卡牌在最上层

        // 2. 定义目标几何形状（实际槽位的几何形状，映射到 CardStoreWidget 坐标系）
        QRect targetRect = targetSlot->mapTo(this, targetSlot->rect());

        // 3. 创建动画
        QPropertyAnimation* animation = new QPropertyAnimation(animatedCard, "geometry", this);
        animation->setDuration(500); // 0.5 秒
        animation->setStartValue(animatedCard->geometry());
        animation->setEndValue(targetRect);
        animation->setEasingCurve(QEasingCurve::OutQuad); // 缓出曲线

        // 4. 将动画数据加入队列
        CardAnimationData data;
        data.cardData = card;
        data.targetSlotPos = pos;
        data.animation = animation;
        data.animatedCardWidget = animatedCard;
        data.isRemovalAnimation = false;
        m_animationQueue.enqueue(data);

        // 连接动画完成信号
        connect(animation, &QPropertyAnimation::finished, this, &CardStoreWidget::animationFinished);

        // 如果没有其他动画正在进行，则启动下一个动画
        if (!m_animationInProgress) {
            startNextAnimation();
        }
        qDebug() << "Card added to store slot:" << card->getName() << "at pos:" << pos;
    }
}

void CardStoreWidget::onCardRemoved(CardStore* store, Card* card, int pos)
{
    if (!m_store || store != m_store) return;

    if (pos == 0) { // 卡牌从供应堆移除
        SlotWidget* supplySlot = m_slots[0];
        // 如果供应堆变空，移除显示的卡牌背面
        if (m_store->getSupplyCount() == 0 && !supplySlot->isEmpty()) {
            CardWidget* removedBackCard = supplySlot->popCard();
            if (removedBackCard) {
                removedBackCard->deleteLater(); // 删除这个虚拟的卡牌背面
            }
        }
        // 更新供应堆的数量显示
        supplySlot->setDisplayedCount(m_store->getSupplyCount());
        qDebug() << "Card removed from supply pile (data only):" << card->getName() << "New count:" << m_store->getSupplyCount();

    } else if (pos >= 1 && pos < m_slots.size()) { // 卡牌从普通商店槽位移除 (购买)
        SlotWidget* sourceSlot = m_slots[pos];

        // 1. 从 SlotWidget 中弹出 CardWidget
        CardWidget* animatedCard = sourceSlot->popCard();
        if (!animatedCard) {
            qWarning() << "CardStoreWidget: Attempted to remove card from empty slot" << pos;
            return;
        }

        // 2. 将 CardWidget 的父对象设置为 CardStoreWidget 以进行动画
        animatedCard->setParent(this);
        // 设置其初始位置为它在槽位中的位置（相对于 CardStoreWidget）
        animatedCard->setGeometry(sourceSlot->mapTo(this, sourceSlot->rect()));
        animatedCard->show();
        animatedCard->raise();

        // 3. 定义目标几何形状（例如，屏幕底部中心，离开屏幕）
        QRect targetRect = QRect(width() / 2 - animatedCard->width() / 2, height() + 50, animatedCard->width(), animatedCard->height());

        // 4. 创建动画
        QPropertyAnimation* animation = new QPropertyAnimation(animatedCard, "geometry", this);
        animation->setDuration(500);
        animation->setStartValue(animatedCard->geometry());
        animation->setEndValue(targetRect);
        animation->setEasingCurve(QEasingCurve::InQuad); // 缓入曲线 (加速离开)

        // 5. 将动画数据加入队列
        CardAnimationData data;
        data.cardData = card; // 存储卡牌数据以供参考
        data.targetSlotPos = pos; // 源槽位位置
        data.animation = animation;
        data.animatedCardWidget = animatedCard;
        data.isRemovalAnimation = true;
        m_animationQueue.enqueue(data);

        // 连接动画完成信号
        connect(animation, &QPropertyAnimation::finished, this, &CardStoreWidget::animationFinished);

        // 如果没有其他动画正在进行，则启动下一个动画
        if (!m_animationInProgress) {
            startNextAnimation();
        }
        qDebug() << "Card removed from store slot:" << card->getName() << "from pos:" << pos;
    }
}

void CardStoreWidget::onSupplyCountChanged(CardStore* store, int newCount)
{
    if (!m_store || store != m_store) return;
    qDebug() << "Supply count changed to:" << newCount;
    m_slots[0]->setDisplayedCount(newCount);

    // 根据新的供应数量，决定是否显示供应堆的卡牌背面
    if (newCount > 0 && m_slots[0]->isEmpty()) {
        Card* dummyCard = new Card("Dummy Back", 0, 0, 0, "", Color::None, CardType::Building, this);
        m_slots[0]->pushCard(new CardWidget(dummyCard, m_slots[0]));
    } else if (newCount == 0 && !m_slots[0]->isEmpty()) {
        CardWidget* removedBackCard = m_slots[0]->popCard();
        if (removedBackCard) {
            removedBackCard->deleteLater();
        }
    }
}

void CardStoreWidget::startNextAnimation()
{
    if (m_animationQueue.isEmpty()) {
        m_animationInProgress = false;
        return;
    }

    m_animationInProgress = true;
    CardAnimationData data = m_animationQueue.head(); // 获取队列头部数据，但不移除
    data.animation->start();
}

void CardStoreWidget::animationFinished()
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
        SlotWidget* targetSlot = m_slots[data.targetSlotPos];
        // 在推入之前隐藏，SlotWidget 会负责显示
        data.animatedCardWidget->hide();
        targetSlot->pushCard(data.animatedCardWidget);
        data.animatedCardWidget->setParent(targetSlot); // 重新设置父对象
        qDebug() << "Card added animation finished for:" << data.cardData->getName() << "to slot:" << data.targetSlotPos;
    } else { // 卡牌移除动画完成
        // 删除动画用的 CardWidget，因为它已经离开场景
        data.animatedCardWidget->deleteLater();
        qDebug() << "Card removed animation finished for:" << data.cardData->getName() << "from slot:" << data.targetSlotPos;
    }

    // 清理动画对象
    data.animation->deleteLater();

    // 启动队列中的下一个动画
    startNextAnimation();
}

void CardStoreWidget::onSlotTopCardClicked(Card* clickedCard)
{
    // 确定是哪个 SlotWidget 发出了信号
    SlotWidget* clickedSlot = qobject_cast<SlotWidget*>(sender());
    if (!clickedSlot) return;

    int slotIndex = m_slots.indexOf(clickedSlot);
    if (slotIndex != -1) {
        qDebug() << "Card clicked in slot:" << slotIndex << "Card Name:" << clickedCard->getName();
        // 在这里，您通常会向更高级别的控制器（例如 CardStoreAreaWidget 或 GameController）
        // 发出信号，以处理购买逻辑。
        // 示例：emit cardPurchased(clickedCard, slotIndex);
    }
}
