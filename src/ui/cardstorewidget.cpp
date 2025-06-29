#include "cardstorewidget.h"
#include <QDebug>
#include <QPixmap>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>
#include <QRegularExpression> // 包含 QRegularExpression 头文件
#include <QPauseAnimation>    // 包含 QPauseAnimation 头文件
#include <QTimer>             // 包含 QTimer 头文件

CardStoreWidget::CardStoreWidget(CardStore* store, QWidget* parent)
    : QWidget(parent)
    , m_store(store)
    , m_supplyPileLabel(new QLabel(this))
    , m_mainLayout(new QHBoxLayout(this))
    , m_animatedCardLabel(new QLabel(this))
    , m_animationInProgress(false) // 初始化为 false
{
    setupUI();

    m_animatedCardLabel->hide();
    m_animatedCardLabel->setFixedSize(100, 150);

    if (m_store) {
        connect(m_store, &CardStore::storeInit, this, &CardStoreWidget::onStoreInit);
        connect(m_store, &CardStore::cardAdded, this, &CardStoreWidget::onCardAdded);
    }

    setMinimumHeight(180);
}

CardStoreWidget::~CardStoreWidget()
{
    // 子部件已设置为当前部件的父对象，会在当前部件析构时自动删除。
}

void CardStoreWidget::setupUI()
{
    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(10);

    m_supplyPileLabel->setFixedSize(80, 120);
    m_supplyPileLabel->setAlignment(Qt::AlignCenter);
    m_supplyPileLabel->setWordWrap(true);
    m_supplyPileLabel->setStyleSheet("border: 1px solid gray; background-color: #e0e0e0; font-weight: bold;");
    m_mainLayout->addWidget(m_supplyPileLabel);

    for (int i = 0; i < m_store->getSlotNum(); ++i) {
        QWidget* slotWidget = createEmptySlotWidget();
        m_cardSlots.append(slotWidget);
        m_mainLayout->addWidget(slotWidget);
    }
    m_mainLayout->addStretch();
}

QWidget* CardStoreWidget::createEmptySlotWidget()
{
    QWidget* container = new QWidget(this);
    container->setFixedSize(100, 150);
    container->setStyleSheet("border: 1px dashed gray; background-color: #f0f0f0;");

    QVBoxLayout* layout = new QVBoxLayout(container);
    layout->setContentsMargins(0,0,0,0);
    QLabel* emptyTextLabel = new QLabel("Empty Slot", container);
    emptyTextLabel->setAlignment(Qt::AlignCenter);
    emptyTextLabel->setStyleSheet("color: gray;");
    layout->addWidget(emptyTextLabel);

    return container;
}

void CardStoreWidget::setSlotContent(int pos, Card* card)
{
    if (pos < 0 || pos >= m_cardSlots.size()) {
        qWarning() << "Invalid slot position:" << pos;
        return;
    }

    QWidget* slotContainer = m_cardSlots[pos];
    QLayout* layout = slotContainer->layout();
    if (!layout) {
        layout = new QVBoxLayout(slotContainer);
        layout->setContentsMargins(0,0,0,0);
    }

    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->hide();
            item->widget()->setParent(nullptr);
            item->widget()->deleteLater();
        }
        delete item;
    }

    if (card) {
        CardWidget* cardWidget = new CardWidget(card, slotContainer);
        layout->addWidget(cardWidget);
        cardWidget->show();
        slotContainer->setStyleSheet("border: none; background-color: transparent;");
    } else {
        QLabel* emptyTextLabel = new QLabel("Empty Slot", slotContainer);
        emptyTextLabel->setAlignment(Qt::AlignCenter);
        emptyTextLabel->setStyleSheet("color: gray;");
        layout->addWidget(emptyTextLabel);
        slotContainer->setStyleSheet("border: 1px dashed gray; background-color: #f0f0f0;");
    }
}

void CardStoreWidget::onStoreInit(CardStore* store, int supplyPileNum, QList<QList<Card*>> slot)
{
    if (store != m_store) return;

    m_supplyPileLabel->setText(QString("Supply Pile\n(%1)").arg(supplyPileNum));

    for (int i = 0; i < m_cardSlots.size(); ++i) {
        if (i < slot.size() && !slot[i].isEmpty()) {
            setSlotContent(i, slot[i].first());
        } else {
            setSlotContent(i, nullptr);
        }
    }
}

void CardStoreWidget::onCardAdded(CardStore* store, Card* card, int pos)
{
    if (store != m_store) return;
    if (!card) return;

    m_animationQueue.enqueue(qMakePair(card, pos));

    if (!m_animationInProgress) {
        startNextAnimation();
    }
}

void CardStoreWidget::startNextAnimation()
{
    if (m_animationQueue.isEmpty()) {
        m_animationInProgress = false;
        return;
    }

    m_animationInProgress = true;
    QPair<Card*, int> animationData = m_animationQueue.dequeue();
    Card* card = animationData.first;
    int pos = animationData.second;

    // 1. 创建一个临时 CardWidget 来渲染卡牌的外观
    CardWidget tempCardWidget(card);
    tempCardWidget.setParent(this);
    tempCardWidget.setFixedSize(m_animatedCardLabel->size());
    tempCardWidget.updateCardUI();
    QPixmap cardPixmap = tempCardWidget.grab();
    tempCardWidget.setParent(nullptr); // 分离临时部件

    // 2. 将 pixmap 设置到动画标签
    m_animatedCardLabel->setPixmap(cardPixmap.scaled(m_animatedCardLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_animatedCardLabel->setScaledContents(true);

    // 3. 确定动画的起始和结束位置 (相对于 CardStoreWidget 自身)
    QRect supplyRect = m_supplyPileLabel->geometry();
    QRect targetRect = m_cardSlots[pos]->geometry();

    QPoint animatedLabelHalfSize(m_animatedCardLabel->width()/2, m_animatedCardLabel->height()/2);

    QPoint localStartPos;
    localStartPos.setX(supplyRect.center().x() - animatedLabelHalfSize.x());
    localStartPos.setY(supplyRect.bottom() - m_animatedCardLabel->height() - 5);

    QPoint localEndPos = targetRect.center() - animatedLabelHalfSize;

    qDebug() << card->getName() << "开始移动，目标槽位:" << pos
             << "，开始位置(局部):" << localStartPos
             << "，结束位置(局部):" << localEndPos;
    qDebug() << "供应堆标签几何信息:" << supplyRect;
    qDebug() << "目标槽位几何信息:" << targetRect;
    qDebug() << "CardStoreWidget 几何信息:" << this->geometry();

    m_animatedCardLabel->move(localStartPos);
    m_animatedCardLabel->show();

    // 4. 创建并启动动画组
    QPropertyAnimation* moveAnimation = new QPropertyAnimation(m_animatedCardLabel, "pos", this);
    moveAnimation->setDuration(700); // 0.7 秒，更平滑
    moveAnimation->setStartValue(localStartPos);
    moveAnimation->setEndValue(localEndPos);
    moveAnimation->setEasingCurve(QEasingCurve::OutQuad);

    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(m_animatedCardLabel);
    m_animatedCardLabel->setGraphicsEffect(opacityEffect);
    QPropertyAnimation* fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeAnimation->setDuration(300); // 淡出动画时长
    fadeAnimation->setStartValue(1.0);
    fadeAnimation->setEndValue(0.0);
    fadeAnimation->setEasingCurve(QEasingCurve::OutQuad);

    // Step 1: 为整个卡牌动画创建一个 0.5 秒的初始延迟
    QPauseAnimation* initialCardDelay = new QPauseAnimation(500, this); // 0.5 秒延迟

    // Step 2: 为淡出动画创建一个顺序组，包含其自身的延迟
    // 这个延迟确保淡出在并行移动中稍后开始。
    QPauseAnimation* fadeStartDelay = new QPauseAnimation(moveAnimation->duration() - fadeAnimation->duration(), this);
    QSequentialAnimationGroup* fadeSequence = new QSequentialAnimationGroup(this);
    fadeSequence->addAnimation(fadeStartDelay);
    fadeSequence->addAnimation(fadeAnimation);

    // Step 3: 为卡牌的移动和延迟淡出创建一个并行组
    QParallelAnimationGroup* cardMovementAndFade = new QParallelAnimationGroup(this);
    cardMovementAndFade->addAnimation(moveAnimation);
    cardMovementAndFade->addAnimation(fadeSequence); // 将包含延迟的顺序淡出组添加到并行组

    // Step 4: 创建主顺序组，它将编排初始延迟，然后是卡牌动画
    QSequentialAnimationGroup* mainAnimationGroup = new QSequentialAnimationGroup(this);
    mainAnimationGroup->addAnimation(initialCardDelay);
    mainAnimationGroup->addAnimation(cardMovementAndFade);

    // 连接主动画组的完成信号
    connect(mainAnimationGroup, &QAnimationGroup::finished, this, [this, card, pos]() {
        m_animatedCardLabel->hide();
        m_animatedCardLabel->setGraphicsEffect(nullptr); // 移除效果
        setSlotContent(pos, card); // 在槽位中放置实际的卡牌部件

        // 更新供应堆数量。
        QString currentText = m_supplyPileLabel->text();
        qDebug() << "尝试解析供应堆文本: '" << currentText << "'";
        QRegularExpression rx("Supply Pile\\n\$$(\\\\d\+)\\$$");
        QRegularExpressionMatch match = rx.match(currentText);
        if (match.hasMatch()) {
            int currentCount = match.captured(1).toInt();
            m_supplyPileLabel->setText(QString("Supply Pile\n(%1)").arg(currentCount - 1));
            qDebug() << "成功解析。新数量:" << currentCount - 1;
        } else {
            qWarning() << "无法从文本中解析供应堆数量: '" << currentText << "'";
            qWarning() << "正则表达式: " << rx.pattern();
            qWarning() << "原始文本(十六进制): " << currentText.toUtf8().toHex();
        }
        // 动画完成后，启动队列中的下一个动画
        startNextAnimation();
    });

    mainAnimationGroup->start(QAbstractAnimation::DeleteWhenStopped); // 动画停止时自动删除组及其动画
}
