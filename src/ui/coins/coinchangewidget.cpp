#include "CoinChangeWidget.h"
#include <QApplication> // 包含 QApplication 用于测试
#include <QPushButton>  // 包含 QPushButton 用于测试
#include <QVBoxLayout>  // 包含 QVBoxLayout 用于测试
#include <QMainWindow>  // 包含 QMainWindow 用于测试

CoinChangeWidget::CoinChangeWidget(QWidget* parent)
    : QWidget(parent),
    currentChangeCoins(0),
    currentCoinsDisplayed(0),
    maxCoinsToDisplay(0),
    m_player(0) // 默认初始化一个 Player (id=0)
{
    setupUI(); // 初始化UI

    // 初始化定时器
    animationTimer = new QTimer(this);
    animationTimer->setSingleShot(true); // 单次触发
    connect(animationTimer, &QTimer::timeout, this, &CoinChangeWidget::updateCoinAnimation);

    hideTimer = new QTimer(this);
    hideTimer->setSingleShot(true); // 单次触发
    connect(hideTimer, &QTimer::timeout, this, &CoinChangeWidget::hideWidget);

    // 初始状态隐藏 Widget
    hide();
}

CoinChangeWidget::~CoinChangeWidget() {
    // Qt 的父子对象机制会自动删除子对象，所以这里不需要手动删除 mainLayout, amountLabel, coinLabels 中的 QLabel
    // 定时器也因为是 this 的子对象，会在析构时自动删除
}

void CoinChangeWidget::setPlayer(Player* p) {
    m_player = p;
}

void CoinChangeWidget::setupUI() {
    mainLayout = new QGridLayout(this);
    setLayout(mainLayout);

    // 设置金币变化量文本 (amountLabel)
    amountLabel = new QLabel(this);
    QFont amountFont("Segoe UI Emoji", BASE_COIN_FONT_SIZE * AMOUNT_FONT_SIZE_MULTIPLIER);
    amountFont.setBold(true); // 加粗
    amountLabel->setFont(amountFont);
    amountLabel->setAlignment(Qt::AlignBottom | Qt::AlignLeft); // 左下对齐
    // 将 amountLabel 放置在网格的左下角 (最后一行, 第一列)
    mainLayout->addWidget(amountLabel, COINS_PER_COLUMN - 1, 0, Qt::AlignBottom | Qt::AlignLeft);

    // 设置金币图标 (coinLabels)
    QFont coinFont("Segoe UI Emoji", BASE_COIN_FONT_SIZE);
    coinLabels.resize(COINS_PER_COLUMN); // 5行
    for (int row = 0; row < COINS_PER_COLUMN; ++row) {
        coinLabels[row].resize(MAX_COIN_COLUMNS); // 4列
        for (int col = 0; col < MAX_COIN_COLUMNS; ++col) {
            QLabel* coin = new QLabel("🪙", this); // 使用金币 Emoji 字符
            coin->setFont(coinFont);
            coin->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
            coin->hide(); // 初始状态隐藏
            // 金币图标从网格的第二列开始放置 (col + 1), 因为第一列留给 amountLabel
            mainLayout->addWidget(coin, row, col + 1, Qt::AlignBottom | Qt::AlignLeft);
            coinLabels[row][col] = coin;
        }
    }

    // 调整布局间距，使金币和数字更紧凑
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setHorizontalSpacing(2); // 列间距
    mainLayout->setVerticalSpacing(0);   // 行间距
}

void CoinChangeWidget::resetState() {
    animationTimer->stop();
    hideTimer->stop();
    amountLabel->clear(); // 清空数字文本
    // 隐藏所有金币图标
    for (int row = 0; row < COINS_PER_COLUMN; ++row) {
        for (int col = 0; col < MAX_COIN_COLUMNS; ++col) {
            coinLabels[row][col]->hide();
        }
    }
}

void CoinChangeWidget::showChange(int changeCoins, Player* player) {
    // 检查是否是当前 Widget 关联的玩家
    if (!(player == m_player)) { // 使用重载的 operator== 进行比较
        return; // 如果不是此 Widget 关联的玩家，则直接返回，不执行任何操作
    }

    if (changeCoins == 0) {
        hide(); // 变化量为0时直接隐藏并返回
        return;
    }

    resetState(); // 重置 Widget 状态，停止所有动画并隐藏所有元素

    currentChangeCoins = changeCoins;
    currentCoinsDisplayed = 0;

    // 设置金币变化量文本的数字和颜色
    amountLabel->setText(QString::number(currentChangeCoins));
    if (currentChangeCoins > 0) {
        amountLabel->setStyleSheet("color: green;");
    } else {
        amountLabel->setStyleSheet("color: red;");
    }

    if (currentChangeCoins < 0) {
        // 如果是负数，只显示数字，不显示金币动画
        hideTimer->start(HIDE_DELAY_MS); // 1秒后隐藏
    } else {
        // 如果是正数，显示金币动画
        // 计算需要显示的金币数量，最多20个
        maxCoinsToDisplay = qMin(currentChangeCoins, COINS_PER_COLUMN * MAX_COIN_COLUMNS);

        updateCoinAnimation(); // 立即显示第一个金币

        if (maxCoinsToDisplay > 1) { // 如果需要显示多个金币，则启动定时器继续动画
            animationTimer->start(ANIMATION_STEP_MS);
        } else { // 如果只显示一个金币，则无需进一步动画，直接启动隐藏定时器
            hideTimer->start(HIDE_DELAY_MS);
        }
    }

    show(); // 显示 Widget
}

void CoinChangeWidget::updateCoinAnimation() {
    if (currentCoinsDisplayed < maxCoinsToDisplay) {
        // 计算下一个要显示的金币的行和列
        int row = currentCoinsDisplayed % COINS_PER_COLUMN;
        int col = currentCoinsDisplayed / COINS_PER_COLUMN;

        // 确保索引在有效范围内
        if (row < COINS_PER_COLUMN && col < MAX_COIN_COLUMNS) {
            coinLabels[row][col]->show(); // 显示该金币
        }
        currentCoinsDisplayed++; // 已显示金币数量加1

        if (currentCoinsDisplayed < maxCoinsToDisplay) {
            animationTimer->start(ANIMATION_STEP_MS); // 如果还有金币未显示，则继续启动定时器
        } else {
            // 所有金币已显示完毕，启动隐藏定时器
            hideTimer->start(HIDE_DELAY_MS);
        }
    }
}

void CoinChangeWidget::hideWidget() {
    hide(); // 隐藏整个 Widget
    resetState(); // 重置状态，为下次显示做准备
}
