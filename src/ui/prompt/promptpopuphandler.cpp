#include "PromptPopupHandler.h"
#include "MainWindow.h"
#include <QPushButton> // For QPushButton
#include <QVBoxLayout> // For layouts
#include <QHBoxLayout>
#include <QPropertyAnimation>

PromptPopupHandler::PromptPopupHandler(MainWindow* main)
    : PromptHandlerBase(main)
{}

void PromptPopupHandler::handle(const PromptData& pd)
{
    int opId = pd.autoInput;
    QString promptMessage = pd.promptMessage;
    QList<OptionData> options = pd.options;
    int delay = pd.delay;

    // 自动模式
    if (pd.isAutoInput) {
        QTimer::singleShot(500, m_main, [this, opId]() {
            if (m_main->m_animationOverlayWidget)
                m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

            emit responseUserInput(opId);
        });
        return;
    }

    // UI 模式
    QTimer::singleShot(0, m_main, [this, opId, promptMessage, options, delay]() {
        if (!m_main->m_animationOverlayWidget) {
            qWarning() << "Popup (delayed): Animation overlay widget is null.";
            emit responseUserInput(opId);
            return;
        }

        // 启动交互提示
        m_main->m_isInteractivePromptActive = true;
        m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
        m_main->m_animationOverlayWidget->show();

        QWidget* popupContainer = new QWidget(m_main->m_animationOverlayWidget);
        popupContainer->setStyleSheet("background-color: rgba(0, 0, 0, 180); border-radius: 10px;");
        popupContainer->setWindowOpacity(0.0);

        QSize overlaySize = m_main->m_animationOverlayWidget->size();
        int containerWidth = overlaySize.width() * 0.4;
        int containerHeight = overlaySize.height() * 0.3;

        popupContainer->setGeometry(
            (overlaySize.width() - containerWidth) / 2,
            (overlaySize.height() - containerHeight) / 2,
            containerWidth,
            containerHeight
            );

        QVBoxLayout* mainLayout = new QVBoxLayout(popupContainer);
        mainLayout->setAlignment(Qt::AlignCenter);
        mainLayout->setContentsMargins(20, 20, 20, 20);
        mainLayout->setSpacing(20);

        QLabel* messageLabel = new QLabel(promptMessage, popupContainer);
        messageLabel->setStyleSheet("color: white; font-size: 20px; font-weight: bold;");
        messageLabel->setAlignment(Qt::AlignCenter);
        messageLabel->setWordWrap(true);
        mainLayout->addWidget(messageLabel);

        QHBoxLayout* optionsLayout = new QHBoxLayout();
        optionsLayout->setAlignment(Qt::AlignCenter);
        mainLayout->addLayout(optionsLayout);

        // 清理函数
        auto finalCleanupAndRespond =
            [this, popupContainer_ptr = QPointer<QWidget>(popupContainer)](int selectedId) {
                if (!popupContainer_ptr) {
                    if (m_main->m_animationOverlayWidget)
                        m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

                    m_main->m_isInteractivePromptActive = false;
                    emit responseUserInput(selectedId);
                    return;
                }

                QPropertyAnimation* fadeOutAnim =
                    new QPropertyAnimation(popupContainer_ptr, "windowOpacity", popupContainer_ptr);

                fadeOutAnim->setDuration(300);
                fadeOutAnim->setStartValue(popupContainer_ptr->windowOpacity());
                fadeOutAnim->setEndValue(0.0);
                fadeOutAnim->setEasingCurve(QEasingCurve::InQuad);

                connect(fadeOutAnim, &QPropertyAnimation::finished,
                        popupContainer_ptr,
                        [popupContainer_ptr, selectedId, this]() {
                            if (popupContainer_ptr)
                                popupContainer_ptr->deleteLater();

                            if (m_main->m_animationOverlayWidget)
                                m_main->m_animationOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);

                            m_main->m_isInteractivePromptActive = false;
                            emit responseUserInput(selectedId);
                        });

                fadeOutAnim->start(QAbstractAnimation::DeleteWhenStopped);
            };

        // 创建按钮
        for (const OptionData& option : options) {
            QPushButton* button = new QPushButton(option.name, popupContainer);
            button->setFixedSize(containerWidth * 0.3, containerHeight * 0.2);

            QFont btnFont = button->font();
            btnFont.setPointSize(14);
            button->setFont(btnFont);
            button->setCursor(Qt::PointingHandCursor);
            button->setProperty("optionId", option.id);

            if (option.state == 0) {
                button->setEnabled(false);
                button->setStyleSheet(
                    "QPushButton { background-color: #555; color: #aaa; border: 1px solid #777; border-radius: 5px; }"
                    "QPushButton:disabled { background-color: #555; color: #aaa; }"
                    );
            } else {
                button->setStyleSheet(
                    "QPushButton { background-color: #4CAF50; color: white; border: 1px solid #388E3C; border-radius: 5px; }"
                    "QPushButton:hover { background-color: #66BB6A; }"
                    "QPushButton:pressed { background-color: #388E3C; }"
                    );

                connect(button, &QPushButton::clicked, this, [finalCleanupAndRespond, option]() {
                    finalCleanupAndRespond(option.id);
                });
            }

            optionsLayout->addWidget(button);
        }

        // 渐入动画
        QPropertyAnimation* fadeInAnim =
            new QPropertyAnimation(popupContainer, "windowOpacity", popupContainer);
        fadeInAnim->setDuration(300);
        fadeInAnim->setStartValue(0.0);
        fadeInAnim->setEndValue(1.0);
        fadeInAnim->setEasingCurve(QEasingCurve::OutQuad);
        fadeInAnim->start(QAbstractAnimation::DeleteWhenStopped);

        popupContainer->show();
    });
}
