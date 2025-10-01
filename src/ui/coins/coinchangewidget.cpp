#include "CoinChangeWidget.h"
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QDebug>
#include <QStackedLayout>
#include <QFontMetrics>

CoinChangeWidget::CoinChangeWidget(Player* player, QWidget* gameMainWidget, QWidget* parent)
    : QWidget(parent),
    currentChangeCoins(0),
    animationCurrentValue(0),
    animationTargetValue(0),
    m_player(player),
    m_gameMainWidget(gameMainWidget)
{

    // 设置初始大小策略，允许 Widget 根据内容调整大小，但我们稍后会设置固定大小
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setupUI(); // 初始化UI

    animationTimer = new QTimer(this);
    animationTimer->setSingleShot(false);
    connect(animationTimer, &QTimer::timeout, this, &CoinChangeWidget::updateNumberAnimation);

    hideTimer = new QTimer(this);
    hideTimer->setSingleShot(true);
    connect(hideTimer, &QTimer::timeout, this, &CoinChangeWidget::hideWidget);

    flashTimer = new QTimer(this);
    flashTimer->setSingleShot(true);
    connect(flashTimer, &QTimer::timeout, this, &CoinChangeWidget::hide2xCoinImage);

    hide(); // 初始状态隐藏
}

CoinChangeWidget::~CoinChangeWidget() {
    // Qt 的父子对象机制会自动删除子对象。
}

// 计算 Widget 的固定大小，基于 2 倍金币图像的尺寸
QSize CoinChangeWidget::getWidgetFixedSize() const {
    QFont coin2xFont("Segoe UI Emoji", BASE_AMOUNT_FONT_SIZE * COIN_2X_SIZE_MULTIPLIER);
    QFontMetrics fm(coin2xFont);
    int coinVisualSize = fm.height(); // 2倍金币图像的视觉高度作为参考
    // 为了确保数字和金币都能完全显示且居中，Widget 的固定大小应至少能容纳 2 倍金币图像
    return QSize(coinVisualSize, coinVisualSize);
}

void CoinChangeWidget::setupUI() {
    if (layout()) {
        delete layout();
    }
    m_stackedLayout = new QStackedLayout(this);
    setLayout(m_stackedLayout);
    m_stackedLayout->setStackingMode(QStackedLayout::StackAll); // 确保所有 Widget 都可见

    // 最上层：数字标签
    amountLabel = new QLabel(this);
    QFont amountFont("ExtraBlack", BASE_AMOUNT_FONT_SIZE*TEXT_SIZE_MULTIPLIER, QFont::Bold);
    amountFont.setBold(true);
    amountLabel->setFont(amountFont);
    amountLabel->setAlignment(Qt::AlignCenter);
    amountLabel->setStyleSheet("background: transparent; color: white;");
    m_stackedLayout->addWidget(amountLabel);

    // 中层：2 倍金币图像 (初始隐藏)
    coinImageLabel_2x = new QLabel("🪙", this);
    QFont coin2xFont("YouYuan", BASE_AMOUNT_FONT_SIZE * COIN_2X_SIZE_MULTIPLIER);
    coinImageLabel_2x->setFont(coin2xFont);
    coinImageLabel_2x->setAlignment(Qt::AlignCenter);
    coinImageLabel_2x->setStyleSheet("background: transparent;");
    coinImageLabel_2x->hide(); // 初始隐藏
    m_stackedLayout->addWidget(coinImageLabel_2x);

    // 底层：1.5 倍金币图像
    coinImageLabel_1_5x = new QLabel("🪙", this);
    QFont coin1_5xFont("YouYuan", BASE_AMOUNT_FONT_SIZE * COIN_1_5X_SIZE_MULTIPLIER);
    coinImageLabel_1_5x->setFont(coin1_5xFont);
    coinImageLabel_1_5x->setAlignment(Qt::AlignCenter);
    coinImageLabel_1_5x->setStyleSheet("background: transparent;");
    m_stackedLayout->addWidget(coinImageLabel_1_5x);

    // 设置 CoinChangeWidget 自身的固定大小，基于 2 倍金币图像的尺寸
    setFixedSize(getWidgetFixedSize());
}

void CoinChangeWidget::resetState() {
    animationTimer->stop();
    hideTimer->stop();
    flashTimer->stop(); // ADDED
    coinImageLabel_2x->hide(); // ADDED: 确保 2x 金币隐藏

    amountLabel->clear();
    amountLabel->setStyleSheet("background: transparent; color: white;");
}

void CoinChangeWidget::showChange(Player* player, int amount, int change) {
    if (!(player == m_player)) {
        return;
    }

    if (change == 0) {
        hide();
        return;
    }

    resetState(); // 重置 Widget 状态

    currentChangeCoins = change;
    animationTargetValue = change;
    animationCurrentValue = 0; // 数字动画从 0 开始

    // 设置初始文本
    amountLabel->setText(QString("0"));
    amountLabel->setStyleSheet("color: white;");

    // 动态定位 Widget
    if (m_gameMainWidget && parentWidget() && m_coinChangePosFunc) {
        int gameMainWidth = m_gameMainWidget->width();
        int gameMainHeight = m_gameMainWidget->height();

        QPoint calculatedPosInGameMain = m_coinChangePosFunc(gameMainWidth, gameMainHeight);
        QPoint globalPos = m_gameMainWidget->mapToGlobal(calculatedPosInGameMain);
        QPoint posInOverlay = parentWidget()->mapFromGlobal(globalPos);

        // 将 CoinChangeWidget 的中心对齐到计算出的位置
        // 注意：getWidgetFixedSize() 返回的是整个 Widget 的尺寸，不是金币图像的尺寸
        QSize widgetSize = getWidgetFixedSize();
        move(posInOverlay - QPoint(widgetSize.width() / 2, widgetSize.height() / 2));
        //qDebug() << "CoinChangeWidget for player" << m_player->getName() << "moved to" << posInOverlay << "in overlay. GameMain size:" << gameMainWidth << "x" << gameMainHeight;
    } else {
        qWarning() << "CoinChangeWidget: Missing m_gameMainWidget, parentWidget, or m_coinChangePosFunc for dynamic positioning.";
    }

    show(); // 显示 Widget

    // 启动数字动画
    animationTimer->start(ANIMATION_STEP_MS);

}

void CoinChangeWidget::updateNumberAnimation() {
    int step = (animationTargetValue > 0) ? 1 : -1;

    if (animationCurrentValue != animationTargetValue) {
        animationCurrentValue += step;

        // 更新 amountLabel 文本和颜色
        if (animationCurrentValue > 0) {
            amountLabel->setText(QString("+%1").arg(animationCurrentValue));
            amountLabel->setStyleSheet("color: white;");
        } else if (animationCurrentValue < 0) {
            amountLabel->setText(QString("%1").arg(animationCurrentValue));
            amountLabel->setStyleSheet("color: #C62828;");
        } else { // animationCurrentValue == 0
            amountLabel->setText(QString("0"));
            amountLabel->setStyleSheet("color: white;");
        }

        // 每次数字增加变化时，闪烁 2x 金币
        if(animationCurrentValue>0)
            flash2xCoinImage();


        if (animationCurrentValue == animationTargetValue) {
            animationTimer->stop();
            hideTimer->start(HIDE_DELAY_MS); // 所有数字已显示完毕，启动隐藏定时器
        }
    } else {
        animationTimer->stop(); // 理论上不应该发生，作为安全措施
        hideTimer->start(HIDE_DELAY_MS);
    }
}

void CoinChangeWidget::flash2xCoinImage() {
    coinImageLabel_2x->show(); // 显示 2x 金币图像
    //m_stackedLayout->setCurrentWidget(coinImageLabel_2x); // 确保 2x 金币在 1.5x 金币之上

    flashTimer->stop(); // 停止任何正在进行的闪烁定时器
    flashTimer->start(FLASH_DURATION_MS); // 35ms 后隐藏 2x 金币
}

void CoinChangeWidget::hide2xCoinImage() {
    coinImageLabel_2x->hide(); // 隐藏 2x 金币图像
    //m_stackedLayout->setCurrentWidget(coinImageLabel_1_5x); // 恢复显示 1.5x 金币
}

void CoinChangeWidget::hideWidget() {
    hide();
    resetState();
}
