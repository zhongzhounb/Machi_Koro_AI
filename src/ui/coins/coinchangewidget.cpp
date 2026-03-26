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
    if (!(player == m_player) || change == 0) {
        hide();
        return;
    }

    resetState();

    currentChangeCoins = change;
    animationTargetValue = change;
    animationCurrentValue = 0;

    // --- 核心逻辑修改：计算动态间隔 ---
    int absChange = qAbs(change);
    int timerInterval = ANIMATION_STEP_MS; // 默认值 (假设原本是 50ms 或 100ms)

    if (absChange > 8) {
        // 如果变化超过 8，目标是总时长固定在 8 个基准步长的时间内完成
        // 公式：新间隔 = (基础间隔 * 8) / 总变化量
        timerInterval = (ANIMATION_STEP_MS * 8) / absChange;

        // 安全检查：防止间隔为 0 导致定时器过快
        if (timerInterval < 10) timerInterval = 10;
    }

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


    this->raise();
    show();

    // 使用计算出的动态间隔启动
    animationTimer->start(timerInterval);
}

void CoinChangeWidget::updateNumberAnimation() {
    int absTarget = qAbs(animationTargetValue);
    int step = (animationTargetValue > 0) ? 1 : -1;

    if (animationCurrentValue != animationTargetValue) {
        animationCurrentValue += step;

        // 更新 UI 文本
        if (animationCurrentValue > 0) {
            amountLabel->setText(QString("+%1").arg(animationCurrentValue));
            amountLabel->setStyleSheet("color: white;");

            // --- 核心逻辑修改：控制闪烁频率 ---
            // 如果总数 > 8，每隔 (总数/8) 步闪烁一次
            if (absTarget <= 8) {
                flash2xCoinImage(); // 8元以内，每步都闪
            } else {
                int flashInterval = absTarget / 8;
                if (qAbs(animationCurrentValue) % flashInterval == 0) {
                    flash2xCoinImage(); // 超过8元，等间距闪烁，总计约8次
                }
            }
        } else {
            amountLabel->setText(QString("%1").arg(animationCurrentValue));
            amountLabel->setStyleSheet("color: #C62828;");
            // 扣钱逻辑：不执行 flash2xCoinImage()
        }

        if (animationCurrentValue == animationTargetValue) {
            animationTimer->stop();
            hideTimer->start(HIDE_DELAY_MS);
        }
    } else {
        animationTimer->stop();
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
