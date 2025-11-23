#include "PromptSelectCardHandler.h"
#include "MainWindow.h"
#include "cards/cardwidget.h"
#include "cards/slotwidget.h"
#include "players/playerareawidget.h"
#include "cards/cardstoreareawidget.h"
#include "card.h"
#include "dices/diceareawidget.h"
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QGraphicsColorizeEffect>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

PromptSelectCardHandler::PromptSelectCardHandler(MainWindow* main)
    : PromptHandlerBase(main)
{}

void PromptSelectCardHandler::handle(const PromptData& pd)
{
    // 自动选择：只延迟响应
    if (pd.isAutoInput) {
        QTimer::singleShot(500, m_main, [this, opId = pd.autoInput]() {
            emit responseUserInput(opId);
        });
        return;
    }

    // 显示等待提示
    m_main->showWaitCurtain(pd.promptMessage);

    QTimer::singleShot(0, m_main, [this, pd]() {
        if (!m_main->m_animationOverlayWidget) {
            qWarning() << "SelectCard (delayed): Animation overlay widget is null. Emitting autoInput.";
            emit responseUserInput(pd.autoInput);
            return;
        }

        // ========= 计算骰子区域位置 =========
        QRect diceRect = m_main->m_mainDiceAreaWidget->geometry();
        QWidget* gameMain = m_main->m_gameMainWidget;

        QPoint diceTopLeftGlobal = gameMain->mapToGlobal(diceRect.topLeft());
        QPoint diceInOverlay = m_main->m_animationOverlayWidget->mapFromGlobal(diceTopLeftGlobal);

        // ========= 创建按钮容器 =========
        QWidget* buttonContainer = new QWidget(gameMain);
        buttonContainer->setStyleSheet("background-color: rgba(0, 0, 0, 150); border-radius: 10px;");
        buttonContainer->setGeometry(diceRect);

        int actualW = diceRect.width();
        int actualH = diceRect.height();

        QVBoxLayout* buttonLayout = new QVBoxLayout(buttonContainer);
        buttonLayout->setAlignment(Qt::AlignCenter);
        buttonLayout->setContentsMargins(10, 10, 10, 10);
        buttonLayout->setSpacing(5);

        // ========= 存储 effect / animation =========
        auto detachedOriginalEffects_ptr =
            QSharedPointer<QMap<QPointer<CardWidget>, QPointer<QGraphicsEffect>>>::create();
        auto activeAnimations_ptr =
            QSharedPointer<QMap<QPointer<QGraphicsEffect>, QPointer<QPropertyAnimation>>>::create();
        auto activeCardClickConnections_ptr =
            QSharedPointer<QList<QMetaObject::Connection>>::create();

        // ========= 清理 + 响应 回调 =========
        auto finalCleanupAndRespond =
            [this,
             buttonContainer_ptr = QPointer<QWidget>(buttonContainer),
             detachedOriginalEffects_ptr,
             activeAnimations_ptr,
             activeCardClickConnections_ptr](int selectedId) mutable
        {
            auto& detachedOriginalEffects = *detachedOriginalEffects_ptr;
            auto& activeAnimations = *activeAnimations_ptr;
            auto& activeConnections = *activeCardClickConnections_ptr;

            // 停止动画
            for (auto anim_ptr : activeAnimations.values()) {
                if (anim_ptr) {
                    anim_ptr->stop();
                    anim_ptr->deleteLater();
                }
            }
            activeAnimations.clear();

            // 恢复 cardWidget 状态
            for (auto cw_ptr : detachedOriginalEffects.keys()) {
                if (!cw_ptr) continue;

                auto original = detachedOriginalEffects.value(cw_ptr);
                if (original) {
                    original->setParent(cw_ptr);
                    cw_ptr->setGraphicsEffect(original);
                } else {
                    cw_ptr->setGraphicsEffect(nullptr);
                }

                cw_ptr->setStyleSheet("");
                cw_ptr->setEnabled(true);
                cw_ptr->setCursor(Qt::ArrowCursor);
                cw_ptr->setToolTip("");

                QObject::connect(cw_ptr, &CardWidget::requestShowDetailedCard,
                                 m_main, &MainWindow::showDetailedCard);
                QObject::connect(cw_ptr, &CardWidget::requestHideDetailedCard,
                                 m_main, &MainWindow::hideDetailedCard);
            }
            detachedOriginalEffects.clear();

            // 断开所有 click 信号
            for (auto& con : activeConnections)
                QObject::disconnect(con);
            activeConnections.clear();

            if (buttonContainer_ptr)
                buttonContainer_ptr->deleteLater();

            if (m_main->m_animationOverlayWidget)
                m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            emit responseUserInput(selectedId);
        };

        // ========= 收集所有 SlotWidget =========
        QList<QPointer<SlotWidget>> allSlotWidgets;

        for (auto list : m_main->m_cardStoreArea->m_storeToSlotsMap.values())
            for (auto s : list)
                allSlotWidgets.append(s);

        for (auto area : m_main->m_playerToCardAreaMap.values())
            for (auto s : area->getSlots())
                allSlotWidgets.append(s);

        for (auto area : m_main->m_playerToLandmarkAreaMap.values())
            for (auto s : area->getSlots())
                allSlotWidgets.append(s);

        // ========= 处理所有 Option =========
        for (const OptionData& option : pd.options) {

            CardWidget* foundCardWidget = nullptr;

            // 找对应卡牌
            for (auto slot_ptr : allSlotWidgets) {
                if (!slot_ptr || !slot_ptr->topCard()) continue;
                Card* card = slot_ptr->topCard()->getCard();
                if (card && card->getId() == option.id) {
                    foundCardWidget = slot_ptr->topCard();
                    break;
                }
            }

            if (foundCardWidget) {
                // 分离原来的 effect
                QGraphicsEffect* original = foundCardWidget->graphicsEffect();
                if (original) {
                    original->setParent(nullptr);
                    detachedOriginalEffects_ptr->insert(foundCardWidget, original);
                } else {
                    detachedOriginalEffects_ptr->insert(foundCardWidget, nullptr);
                }

                QObject::disconnect(foundCardWidget, &CardWidget::requestShowDetailedCard,
                                    m_main, &MainWindow::showDetailedCard);
                QObject::disconnect(foundCardWidget, &CardWidget::requestHideDetailedCard,
                                    m_main, &MainWindow::hideDetailedCard);

                /* ====================
                   状态 0：不可点击（变暗）
                ===================== */
                if (option.state == 0) {
                    auto eff = new QGraphicsColorizeEffect(foundCardWidget);
                    eff->setColor(QColor(0, 0, 0));
                    eff->setStrength(0.3);

                    foundCardWidget->setGraphicsEffect(eff);
                    foundCardWidget->setEnabled(false);
                    foundCardWidget->setCursor(Qt::ForbiddenCursor);
                    foundCardWidget->setToolTip(option.unClickMessage);
                }

                /* ====================
                   状态 1：闪烁 + 点击
                ===================== */
                else if (option.state == 1) {
                    auto eff = new QGraphicsOpacityEffect(foundCardWidget);
                    foundCardWidget->setGraphicsEffect(eff);

                    auto anim = new QPropertyAnimation(eff, "opacity", eff);
                    anim->setDuration(800);
                    anim->setStartValue(0.5);
                    anim->setEndValue(1.0);
                    anim->setEasingCurve(QEasingCurve::InOutQuad);
                    anim->setLoopCount(-1);
                    anim->start(QAbstractAnimation::DeleteWhenStopped);

                    activeAnimations_ptr->insert(eff, anim);

                    foundCardWidget->setCursor(Qt::PointingHandCursor);

                    auto conn = QObject::connect(foundCardWidget, &CardWidget::clicked,
                                                 m_main, [finalCleanupAndRespond, option]() mutable {
                                                     finalCleanupAndRespond(option.id);
                                                 });

                    activeCardClickConnections_ptr->append(conn);
                }

                /* ====================
                   状态 2：高亮 + 点击
                ===================== */
                else if (option.state == 2) {
                    auto eff = new QGraphicsDropShadowEffect(foundCardWidget);
                    eff->setColor(QColor(255, 255, 0));
                    eff->setBlurRadius(20);
                    eff->setOffset(0, 0);

                    foundCardWidget->setGraphicsEffect(eff);
                    foundCardWidget->setCursor(Qt::PointingHandCursor);

                    auto conn = QObject::connect(foundCardWidget, &CardWidget::clicked,
                                                 m_main, [finalCleanupAndRespond, option]() mutable {
                                                     finalCleanupAndRespond(option.id);
                                                 });

                    activeCardClickConnections_ptr->append(conn);
                }
            }
            else {
                // ================== 使用按钮 ==================
                QPushButton* btn = new QPushButton(option.name, buttonContainer);
                btn->setFixedSize(actualW * 0.5, actualH * 0.5);

                QFont f = btn->font();
                f.setPointSize(actualH / 5);
                btn->setFont(f);

                btn->setStyleSheet(
                    "QPushButton { background-color: #4CAF50; color: white; border: 1px solid #388E3C; "
                    "border-radius: 5px; }"
                    "QPushButton:hover { background-color: #66BB6A; }"
                    "QPushButton:pressed { background-color: #388E3C; }"
                    );

                auto conn = QObject::connect(btn, &QPushButton::clicked,
                                             m_main, [finalCleanupAndRespond, option]() mutable {
                                                 finalCleanupAndRespond(option.id);
                                             });

                buttonLayout->addWidget(btn);
            }
        }

        // ==================== 按钮淡入动画 ====================
        if (buttonLayout->count() > 0) {
            auto eff = new QGraphicsOpacityEffect(buttonContainer);
            buttonContainer->setGraphicsEffect(eff);
            eff->setOpacity(0.0);

            auto anim = new QPropertyAnimation(eff, "opacity", eff);
            anim->setDuration(300);
            anim->setStartValue(0.0);
            anim->setEndValue(1.0);
            anim->setEasingCurve(QEasingCurve::OutQuad);
            anim->start(QAbstractAnimation::DeleteWhenStopped);

            buttonContainer->show();
            buttonContainer->raise();
        } else {
            buttonContainer->deleteLater();
        }

    });
}
