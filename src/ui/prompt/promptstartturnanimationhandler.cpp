#include "PromptStartTurnAnimationHandler.h"
#include "MainWindow.h"

PromptStartTurnAnimationHandler::PromptStartTurnAnimationHandler(MainWindow* main)
    : PromptHandlerBase(main)
{}


void PromptStartTurnAnimationHandler::handle(const PromptData& pd)
{
    int opId = pd.autoInput;
    QString message = pd.promptMessage;

    QTimer::singleShot(0, m_main, [this, opId, message]() {

        if (!m_main->m_animationOverlayWidget) {
            emit responseUserInput(opId);
            return;
        }

        QWidget* overlay = m_main->m_animationOverlayWidget;
        QSize overlaySize = overlay->size();
        int curtainWidth = overlaySize.width();
        int targetCurtainHeight = overlaySize.height() * 0.15;

        int startY = overlaySize.height() / 2;
        int endY = (overlaySize.height() - targetCurtainHeight) / 2;

        QWidget* curtain = new QWidget(overlay);
        curtain->setStyleSheet("background-color: rgba(0,0,0,150);");
        curtain->setGeometry(0, startY, curtainWidth, 0);
        curtain->setAttribute(Qt::WA_TransparentForMouseEvents);
        curtain->setAttribute(Qt::WA_StaticContents);
        curtain->show();

        QPointer<QWidget> curtainPtr = curtain;

        // ----- 构造带字距文本 -----
        QString spacedMessage;
        for (int i = 0; i < message.size(); ++i) {
            spacedMessage += message[i];
            if (i < message.size() - 1)
                spacedMessage += " ";
        }

        QLabel* msgLabel = new QLabel(spacedMessage, curtain);
        msgLabel->setAlignment(Qt::AlignCenter);
        msgLabel->setFont(QFont("YouYuan", overlaySize.height() / 25, QFont::Bold));
        msgLabel->setGeometry(0, 0, curtainWidth, targetCurtainHeight);

        auto* msgOpacity = new QGraphicsOpacityEffect(msgLabel);
        msgOpacity->setOpacity(0.0);
        msgLabel->setGraphicsEffect(msgOpacity);
        msgLabel->show();

        QPointer<QGraphicsOpacityEffect> msgOpacityPtr = msgOpacity;

        // --------- 动画组 ---------
        auto* group = new QSequentialAnimationGroup(curtain);

        // 黑幕展开
        auto* expand = new QPropertyAnimation(curtain, "geometry");
        expand->setDuration(300);
        expand->setStartValue(QRect(0, startY, curtainWidth, 0));
        expand->setEndValue(QRect(0, endY, curtainWidth, targetCurtainHeight));
        expand->setEasingCurve(QEasingCurve::OutQuad);
        group->addAnimation(expand);

        // 文本渐入
        auto* fadeIn = new QPropertyAnimation(msgOpacity, "opacity");
        fadeIn->setDuration(200);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->setEasingCurve(QEasingCurve::OutQuad);
        group->addAnimation(fadeIn);

        group->addAnimation(new QPauseAnimation(1400)); // 停顿

        // 文本渐出
        auto* fadeOut = new QPropertyAnimation(msgOpacity, "opacity");
        fadeOut->setDuration(200);
        fadeOut->setStartValue(1.0);
        fadeOut->setEndValue(0.0);
        fadeOut->setEasingCurve(QEasingCurve::InQuad);
        group->addAnimation(fadeOut);

        // 黑幕收缩
        auto* collapse = new QPropertyAnimation(curtain, "geometry");
        collapse->setDuration(300);
        collapse->setStartValue(QRect(0, endY, curtainWidth, targetCurtainHeight));
        collapse->setEndValue(QRect(0, startY, curtainWidth, 0));
        collapse->setEasingCurve(QEasingCurve::InQuad);
        group->addAnimation(collapse);

        // 动画结束
        connect(group, &QSequentialAnimationGroup::finished, m_main,
                [this, opId, curtainPtr]() {
                    if (curtainPtr)
                        curtainPtr->deleteLater();

                    emit responseUserInput(opId);
                });

        group->start(QAbstractAnimation::DeleteWhenStopped);
    });
}
