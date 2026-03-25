#include "PromptLoadingAnimationHandler.h"
#include "MainWindow.h"
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTimer>
#include <QVBoxLayout>
#include "global.h"

PromptLoadingAnimationHandler::PromptLoadingAnimationHandler(MainWindow* main)
    : PromptHandlerBase(main) {}

void PromptLoadingAnimationHandler::handle(const PromptData& pd) {
    m_opId = pd.autoInput;
    if(m_opId == 2) return;

    // 1. 显示并执行淡入动画
    showBlackCurtain();

    // 2. 立即回调后端逻辑
    responseUserInput(m_opId);

    // 3. 5秒后执行淡出
    QTimer::singleShot(5000, this, [this]() {
        startFadeOutAnimation();
    });
}

void PromptLoadingAnimationHandler::showBlackCurtain() {
    QWidget* overlay = m_main->findChild<QWidget*>("animationOverlayWidget");
    if (!overlay) overlay = m_main;

    m_container = new QWidget(overlay);
    m_container->setGeometry(overlay->rect());
    m_container->setStyleSheet("background-color: black;");

    // 设置透明度效果
    m_opacityEffect = new QGraphicsOpacityEffect(m_container);
    m_container->setGraphicsEffect(m_opacityEffect);
    m_opacityEffect->setOpacity(0.0); // 【关键】初始透明度为0，准备淡入

    // 文字标签
    m_loadingLabel = new QLabel("Loading...", m_container);
    m_loadingLabel->setStyleSheet("color: white; font: bold 28px 'Microsoft YaHei';");
    m_loadingLabel->setFixedWidth(300);
    m_loadingLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout* layout = new QVBoxLayout(m_container);
    layout->addWidget(m_loadingLabel, 0, Qt::AlignCenter);

    m_container->show();
    m_container->raise();

    // --- A. 增加：淡入动画 (0.0 -> 1.0) ---
    QPropertyAnimation* fadeIn = new QPropertyAnimation(m_opacityEffect, "opacity");
    fadeIn->setDuration(500); // 0.5秒淡入
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InCubic);
    fadeIn->start(QAbstractAnimation::DeleteWhenStopped);

    // --- B. 省略号循环动画 ---
    m_dotCount = 3;
    m_dotTimer = new QTimer(this);
    connect(m_dotTimer, &QTimer::timeout, this, [this]() {
        m_dotCount = (m_dotCount + 1) % 4;
        QString dots = "";
        for(int i = 0; i < m_dotCount; ++i) dots += ".";
        m_loadingLabel->setText(QString("Loading%1").arg(dots));
    });
    m_dotTimer->start(500);
}

void PromptLoadingAnimationHandler::startFadeOutAnimation() {
    if (m_isClosing || !m_container) return;
    m_isClosing = true;

    if (m_dotTimer) m_dotTimer->stop();

    // --- 淡出动画 (1.0 -> 0.0) ---
    QPropertyAnimation* fadeOut = new QPropertyAnimation(m_opacityEffect, "opacity");
    fadeOut->setDuration(600);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::OutCubic);

    connect(fadeOut, &QPropertyAnimation::finished, this, [this]() {
        // 3秒后执行淡出
        QTimer::singleShot(3000, this, [this]() {
            responseUserInput(m_opId); // 通知后端加载彻底结束
        });

        if (m_container) {
            m_container->hide();
            m_container->deleteLater();
            m_container = nullptr;
        }
    });

    fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}
