#include "cardstorewidget.h"
#include <QDebug>
#include <QTimer>
#include <QGraphicsOpacityEffect> // For potential fade effects
#include "card.h"
#include "cardwidget.h"
#include "cardstore.h"
#include "slotwidget.h"

// 辅助函数：根据 Card 对象获取其图片路径（如果需要，但动画直接使用 CardWidget）
QString CardStoreWidget::getCardImagePath(Card* card) {
    if (!card) return "";
    // 假设 CardWidget 的 classNameToImagePath 逻辑在此处也适用
    // 注意：这个函数在当前实现中可能没有被直接使用，因为动画直接创建 CardWidget
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
        connect(m_store,&CardStore::supplyCardAdded,this,&CardStoreWidget::onSupplyCardAdded);
        connect(m_store, &CardStore::cardAdded, this, &CardStoreWidget::onCardAdded);

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
            // 如果 animatedCardWidget 还没有被重新父化到 SlotWidget，则删除它
            // 如果已经被重新父化，则由 SlotWidget 或其父级负责删除
            if (data.animatedCardWidget->parent() == this) {
                data.animatedCardWidget->deleteLater(); // 安排删除
            }
        }
    }
    // Qt 的父子机制会处理 m_mainLayout 和 m_slots 内容的删除。
}

void CardStoreWidget::initUI()
{
    if(!m_store){
        qDebug()<<"CardStoreWidget：无store信息";
        return;
    }

    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    //从-1开始，因为有供应堆
    for (int i = -1; i < m_store->getStoreSlotsCount() ; i++) {
        SlotWidget* slot;
        if (i == -1) { // 第一个槽位是供应堆
            slot = new SlotWidget(true, m_store->getName(), this);

        } else { // 剩余槽位是普通商店槽位
            slot = new SlotWidget(false, "", this);
            QList<Card*> cards=m_store->getSlots().at(i);
            for(Card* card:cards)
                slot->pushCard(new CardWidget(card,this));
        }
        m_slots.append(slot);
        m_mainLayout->addWidget(slot);

        // 连接 SlotWidget 的点击信号到 CardStoreWidget 的槽
        connect(slot, &SlotWidget::topCardClickedInSlot, this, &CardStoreWidget::onSlotTopCardClicked);
    }
}


void CardStoreWidget::onCardAdded(CardStore* store, Card* card, int pos)
{
    if (!m_store || store != m_store) return;

    //位移
    pos++;

    // 卡牌添加到普通商店槽位
    SlotWidget* targetSlot = m_slots[pos];
    SlotWidget* supplySlot = m_slots[0]; // 假设动画从供应堆开始

    // 1. 创建一个用于动画的临时 CardWidget，父对象设为 CardStoreWidget
    CardWidget* animatedCard = new CardWidget(card, this);
    // 设置其初始位置为供应堆的几何形状（映射到 CardStoreWidget 坐标系）
    // 假设供应堆的 topCard() 返回的是其显示的卡牌，我们从那里开始动画
    animatedCard->setGeometry(supplySlot->geometry());
    animatedCard->show();
    animatedCard->raise(); // 确保动画卡牌在最上层

    // 2. 定义目标几何形状（实际槽位的几何形状，映射到 CardStoreWidget 坐标系）
    QRect targetRect = targetSlot->rect();

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
    data.isRemovalAnimation = false; // 这是一个添加动画
    m_animationQueue.enqueue(data);

    // 连接动画完成信号
    connect(animation, &QPropertyAnimation::finished, this, &CardStoreWidget::animationFinished);

    // 如果没有其他动画正在进行，则启动下一个动画
    if (!m_animationInProgress) {
        startNextAnimation();
    }
    qDebug() << "Card added to store slot:" << card->getName() << "at pos:" << pos;

}

// onCardRemoved 槽函数已从 .h 文件中移除，因此不再实现

// onSupplyCountChanged 槽函数已从 .h 文件中移除，因此不再实现

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
        // 重新设置父对象，SlotWidget::pushCard 内部已经设置了父对象，这里可以省略
        // data.animatedCardWidget->setParent(targetSlot);
        qDebug() << "Card added animation finished for:" << data.cardData->getName() << "to slot:" << data.targetSlotPos;
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

void CardStoreWidget::onSupplyCardAdded(CardStore* store){
    if(store!=m_store)
        return ;
    m_slots[0]->addCount();
}
