// #include "cardstorewidget.h"
// #include <QDebug>
// #include <QPixmap>
// #include <QSequentialAnimationGroup>
// #include <QParallelAnimationGroup>
// #include <QRegularExpression>
// #include <QPauseAnimation>
// #include <QTimer>
// #include <QHBoxLayout>
// #include <QStackedLayout>
// #include <QFile>

// // 定义牌背图片路径
// const QString CARD_BACK_IMAGE_PATH = ":/resources/images/card/back.png";

// // 新增辅助函数：根据商店名称获取供应堆背景图片路径
// QString storeNameToSupplyPileImagePath(const QString& storeName) {
//     QString fileName = storeName.toLower().replace(" ", "_");
//     QString path = QString(":/resources/images/card/back/%1.png").arg(fileName);

//     qDebug() << "DEBUG: storeNameToSupplyPileImagePath - Attempting to generate path for store:" << storeName;
//     qDebug() << "DEBUG: storeNameToSupplyPileImagePath - Generated file name part:" << fileName;
//     qDebug() << "DEBUG: storeNameToSupplyPileImagePath - Full generated path:" << path;

//     if (!QFile::exists(path)) {
//         qWarning() << "WARNING: storeNameToSupplyPileImagePath - Specific supply pile image file DOES NOT exist in resources:" << path;
//         qWarning() << "WARNING: storeNameToSupplyPileImagePath - Falling back to general CARD_BACK_IMAGE_PATH:" << CARD_BACK_IMAGE_PATH;
//         return CARD_BACK_IMAGE_PATH;
//     }
//     return path;
// }


// CardStoreWidget::CardStoreWidget(CardStore* store, QWidget* parent)
//     : QWidget(parent)
//     , m_store(store)
//     , m_mainLayout(new QHBoxLayout(this))
//     , m_animatedCardWidget(nullptr)
//     , m_animationInProgress(false)
// {
//     setupUI();

//     m_animatedCardWidget = nullptr;

//     if (m_store) {
//         connect(m_store, &CardStore::storeInit, this, &CardStoreWidget::onStoreInit);
//         connect(m_store, &CardStore::cardAdded, this, &CardStoreWidget::onCardAdded);
//     }

//     setMinimumHeight(180);

//     initializeDisplaySlots();
// }

// CardStoreWidget::~CardStoreWidget()
// {
//     if (m_animatedCardWidget) {
//         m_animatedCardWidget->deleteLater();
//     }
// }

// void CardStoreWidget::setupUI()
// {
//     setLayout(m_mainLayout);
//     m_mainLayout->setContentsMargins(10, 10, 10, 10);
//     m_mainLayout->setSpacing(10);
// }

// void CardStoreWidget::initializeDisplaySlots()
// {
//     QLayoutItem *item;
//     while (m_mainLayout->count() > 0) {
//         item = m_mainLayout->takeAt(m_mainLayout->count() - 1);
//         if (item->widget()) {
//             item->widget()->deleteLater();
//         }
//         delete item;
//     }
//     m_displaySlots.clear();

//     QWidget* supplyPileWidget = createSupplyPileWidget();
//     m_displaySlots.append(supplyPileWidget);
//     m_mainLayout->addWidget(supplyPileWidget);

//     int slotNum = m_store ? m_store->getSlotNum() : 0;
//     if (slotNum < 0) slotNum = 0;

//     for (int i = 0; i < slotNum; ++i) {
//         QWidget* cardSlotWidget = createEmptyCardSlotWidget();
//         m_displaySlots.append(cardSlotWidget);
//         m_mainLayout->addWidget(cardSlotWidget);
//     }
//     m_mainLayout->addStretch();
// }

// QWidget* CardStoreWidget::createSupplyPileWidget()
// {
//     QWidget* container = new QWidget(this);
//     container->setFixedSize(100, 150);
//     container->setStyleSheet("border: 1px solid gray;");

//     QStackedLayout* stackLayout = new QStackedLayout(container);
//     stackLayout->setContentsMargins(0,0,0,0);
//     stackLayout->setStackingMode(QStackedLayout::StackAll);

//     QLabel* backImageLabel = new QLabel(container);
//     backImageLabel->setScaledContents(true);
//     backImageLabel->setAlignment(Qt::AlignCenter);
//     backImageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed); // <--- 关键修改：设置为 Fixed
//     backImageLabel->setFixedSize(100, 150); // <--- 关键修改：强制与容器大小一致
//     backImageLabel->setStyleSheet("background-color: #e0e0e0;"); // 默认灰色背景
//     stackLayout->addWidget(backImageLabel);

//     QLabel* countLabel = new QLabel(container);
//     countLabel->setObjectName("countLabel");
//     countLabel->setStyleSheet("background-color: rgba(0, 0, 0, 180); color: white; font-weight: bold; font-size: 14px; border-radius: 5px; padding: 2px;");
//     countLabel->setFixedSize(40, 20);
//     countLabel->setAlignment(Qt::AlignCenter);
//     countLabel->hide();
//     stackLayout->addWidget(countLabel);

//     return container;
// }

// QWidget* CardStoreWidget::createEmptyCardSlotWidget()
// {
//     QWidget* container = new QWidget(this);
//     container->setFixedSize(100, 150);
//     container->setStyleSheet("border: 1px dashed white; background-color: transparent;");

//     QStackedLayout* stackLayout = new QStackedLayout(container);
//     stackLayout->setContentsMargins(0,0,0,0);
//     stackLayout->setStackingMode(QStackedLayout::StackAll);

//     QLabel* countLabel = new QLabel(container);
//     countLabel->setObjectName("countLabel");
//     countLabel->setStyleSheet("background-color: rgba(0, 0, 0, 180); color: white; font-weight: bold; font-size: 14px; border-radius: 5px; padding: 2px;");
//     countLabel->setFixedSize(30, 20);
//     countLabel->setAlignment(Qt::AlignCenter);
//     countLabel->hide();
//     stackLayout->addWidget(countLabel);

//     return container;
// }

// void CardStoreWidget::updateSlotDisplay(int displayPos, Card* card, int count)
// {
//     if (displayPos < 0 || displayPos >= m_displaySlots.size()) {
//         return;
//     }

//     QWidget* slotContainer = m_displaySlots[displayPos];
//     QStackedLayout* stackLayout = qobject_cast<QStackedLayout*>(slotContainer->layout());
//     if (!stackLayout) {
//         return;
//     }

//     QLabel* countLabel = slotContainer->findChild<QLabel*>("countLabel");
//     if (!countLabel) {
//         return;
//     }

//     QLayoutItem *item;
//     for (int i = stackLayout->count() - 1; i >= 0; --i) {
//         item = stackLayout->itemAt(i);
//         if (item->widget() && item->widget() != countLabel) {
//             stackLayout->takeAt(i);
//             item->widget()->hide();
//             item->widget()->setParent(nullptr);
//             item->widget()->deleteLater();
//             delete item;
//         }
//     }

//     if (displayPos == 0) { // 供应堆
//         slotContainer->setStyleSheet("border: 1px solid gray;");
//         QLabel* backImageLabel = qobject_cast<QLabel*>(stackLayout->itemAt(0)->widget());
//         if (backImageLabel) {
//             QString supplyPileImagePath = "";
//             if (m_store) {
//                 supplyPileImagePath = storeNameToSupplyPileImagePath(m_store->getName());
//             }

//             qDebug() << "DEBUG: updateSlotDisplay (Supply Pile) - Attempting to load image from path:" << supplyPileImagePath;
//             QPixmap backImage(supplyPileImagePath);
//             qDebug() << "DEBUG: updateSlotDisplay (Supply Pile) - QPixmap isNull after load:" << backImage.isNull();
//             qDebug() << "DEBUG: updateSlotDisplay (Supply Pile) - Loaded QPixmap size:" << backImage.size();
//             qDebug() << "DEBUG: updateSlotDisplay (Supply Pile) - backImageLabel current size BEFORE setPixmap:" << backImageLabel->size();

//             if (!backImage.isNull()) {
//                 backImageLabel->setPixmap(backImage.scaled(backImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
//                 backImageLabel->setScaledContents(true);
//                 qDebug() << "DEBUG: updateSlotDisplay (Supply Pile) - Successfully set pixmap.";
//             } else {
//                 qWarning() << "WARNING: updateSlotDisplay (Supply Pile) - Failed to load image from:" << supplyPileImagePath << ". Falling back to gray background.";
//                 backImageLabel->clear();
//                 backImageLabel->setStyleSheet("background-color: gray;");
//             }
//             backImageLabel->show();
//             qDebug() << "DEBUG: updateSlotDisplay (Supply Pile) - backImageLabel current size AFTER setPixmap:" << backImageLabel->size();
//         }
//     } else { // 普通卡槽
//         if (card) {
//             CardWidget* cardWidget = new CardWidget(card, slotContainer);
//             stackLayout->insertWidget(0, cardWidget);
//             cardWidget->show();
//             slotContainer->setStyleSheet("border: none; background-color: transparent;");
//         } else {
//             slotContainer->setStyleSheet("border: 1px dashed white; background-color: transparent;");
//         }
//     }

//     if (count > 0 && (displayPos > 0 || displayPos == 0)) {
//         countLabel->setText(QString("×%1").arg(count));
//         countLabel->move(slotContainer->width() - countLabel->width() - 5, 5);
//         countLabel->show();
//         countLabel->raise();
//     } else {
//         countLabel->hide();
//     }
// }


// void CardStoreWidget::onStoreInit(CardStore* store, int supplyPileNum, QList<QList<Card*>> slot)
// {
//     if (store != m_store) return;

//     updateSlotDisplay(0, nullptr, supplyPileNum);

//     for (int i = 0; i < m_store->getSlotNum(); ++i) {
//         Card* topCard = nullptr;
//         int currentCount = 0;
//         if (i < slot.size() && !slot[i].isEmpty()) {
//             topCard = slot[i].first();
//             currentCount = slot[i].size();
//         }
//         updateSlotDisplay(i + 1, topCard, currentCount);
//     }
// }

// void CardStoreWidget::onCardAdded(CardStore* store, Card* card, int pos)
// {
//     if (store != m_store) return;
//     if (!card) return;

//     m_animationQueue.enqueue(qMakePair(card, pos));

//     if (!m_animationInProgress) {
//         startNextAnimation();
//     }
// }

// void CardStoreWidget::startNextAnimation()
// {
//     if (m_animationQueue.isEmpty()) {
//         m_animationInProgress = false;
//         return;
//     }

//     m_animationInProgress = true;
//     QPair<Card*, int> animationData = m_animationQueue.dequeue();
//     Card* card = animationData.first;
//     int pos = animationData.second;

//     if (!card) {
//         m_animationInProgress = false;
//         startNextAnimation();
//         return;
//     }

//     if (m_animatedCardWidget) {
//         m_animatedCardWidget->deleteLater();
//         m_animatedCardWidget = nullptr;
//     }
//     m_animatedCardWidget = new CardWidget(card, this);
//     m_animatedCardWidget->setFixedSize(100, 150);
//     //m_animatedCardWidget->updateData();

//     QRect supplyRect = m_displaySlots[0]->geometry();
//     QRect targetRect = m_displaySlots[pos + 1]->geometry();

//     QPoint animatedWidgetHalfSize(m_animatedCardWidget->width()/2, m_animatedCardWidget->height()/2);

//     QPoint localStartPos;
//     localStartPos.setX(supplyRect.center().x() - animatedWidgetHalfSize.x());
//     localStartPos.setY(supplyRect.bottom() - m_animatedCardWidget->height() - 5);

//     QPoint localEndPos = targetRect.center() - animatedWidgetHalfSize;

//     m_animatedCardWidget->move(localStartPos);
//     m_animatedCardWidget->show();

//     QPropertyAnimation* moveAnimation = new QPropertyAnimation(m_animatedCardWidget, "pos", this);
//     moveAnimation->setDuration(700);
//     moveAnimation->setStartValue(localStartPos);
//     moveAnimation->setEndValue(localEndPos);
//     moveAnimation->setEasingCurve(QEasingCurve::OutQuad);

//     QPauseAnimation* initialCardDelay = new QPauseAnimation(500, this);

//     QParallelAnimationGroup* cardMovementGroup = new QParallelAnimationGroup(this);
//     cardMovementGroup->addAnimation(moveAnimation);

//     QSequentialAnimationGroup* mainAnimationGroup = new QSequentialAnimationGroup(this);
//     mainAnimationGroup->addAnimation(initialCardDelay);
//     mainAnimationGroup->addAnimation(cardMovementGroup);

//     connect(mainAnimationGroup, &QAnimationGroup::finished, this, [this, card, pos]() {
//         m_animatedCardWidget->hide();
//         m_animatedCardWidget->setGraphicsEffect(nullptr);

//         int targetDisplayPos = pos + 1;
//         QWidget* slotContainer = m_displaySlots[targetDisplayPos];
//         QStackedLayout* stackLayout = qobject_cast<QStackedLayout*>(slotContainer->layout());
//         QLabel* countLabel = slotContainer->findChild<QLabel*>("countLabel");

//         QLayoutItem *item;
//         for (int i = stackLayout->count() - 1; i >= 0; --i) {
//             item = stackLayout->itemAt(i);
//             if (item->widget() && item->widget() != countLabel) {
//                 stackLayout->takeAt(i);
//                 item->widget()->hide();
//                 item->widget()->setParent(nullptr);
//                 item->widget()->deleteLater();
//                 delete item;
//             }
//         }

//         if (m_animatedCardWidget) {
//             m_animatedCardWidget->setParent(slotContainer);
//             stackLayout->insertWidget(0, m_animatedCardWidget);
//             m_animatedCardWidget->show();
//             slotContainer->setStyleSheet("border: none; background-color: transparent;");
//             m_animatedCardWidget = nullptr;
//         } else {
//             Card* topCardInSlot = nullptr;
//             int currentSlotCount = 0;
//             if (m_store && pos >= 0 && pos < m_store->getSlots().size()) {
//                 if (!m_store->getSlots()[pos].isEmpty()) {
//                     currentSlotCount = m_store->getSlots()[pos].size();
//                     topCardInSlot = m_store->getSlots()[pos].last();
//                 }
//             }
//             updateSlotDisplay(targetDisplayPos, topCardInSlot, currentSlotCount);
//         }

//         int currentSlotCount = m_store && pos >= 0 && pos < m_store->getSlots().size() ? m_store->getSlots()[pos].size() : 0;
//         if (currentSlotCount > 0 && countLabel) {
//             countLabel->setText(QString("×%1").arg(currentSlotCount));
//             countLabel->move(slotContainer->width() - countLabel->width() - 5, 5);
//             countLabel->show();
//             countLabel->raise();
//         } else if (countLabel) {
//             countLabel->hide();
//         }

//         int supplyCount = m_store ? m_store->getSupplyPileNum() : 0;
//         updateSlotDisplay(0, nullptr, supplyCount);

//         startNextAnimation();
//     });

//     mainAnimationGroup->start(QAbstractAnimation::DeleteWhenStopped);
// }
