#ifndef COINCHANGEWIDGET_H
#define COINCHANGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QTimer>
#include <QFont>
#include <QVector>

class Player;

class CoinChangeWidget : public QWidget {
    Q_OBJECT

public:
    explicit CoinChangeWidget(QWidget* parent = nullptr);
    ~CoinChangeWidget();

    // 设置此 Widget 关联的玩家
    void setPlayer(Player* p);

public slots:
    // 传入金币变化量和玩家信息
    void showChange(int changeCoins, Player* player);

private slots:
    void updateCoinAnimation(); // 更新金币动画，每70ms显示一个新金币
    void hideWidget();          // 隐藏 Widget

private:
    QGridLayout* mainLayout;
    QLabel* amountLabel;
    QVector<QVector<QLabel*>> coinLabels; // 5行 x 4列 的金币QLabel矩阵

    QTimer* animationTimer; // 控制金币逐个显示的定时器
    QTimer* hideTimer;      // 控制Widget最终隐藏的定时器

    int currentChangeCoins;    // 当前的金币变化量
    int currentCoinsDisplayed; // 当前已显示的金币数量
    int maxCoinsToDisplay;     // 最多需要显示的金币数量 (上限20)

    Player* m_player; // 此 Widget 关联的玩家

    // 常量配置
    const int COINS_PER_COLUMN = 5;         // 每列金币数量
    const int MAX_COIN_COLUMNS = 4;         // 最大金币列数
    const int BASE_COIN_FONT_SIZE = 12;     // 金币图标的基础字体大小
    const int AMOUNT_FONT_SIZE_MULTIPLIER = 5; // 变化量数字是金币图标字体大小的倍数
    const int ANIMATION_STEP_MS = 70;       // 每帧动画的间隔时间
    const int HIDE_DELAY_MS = 1000;         // 所有动画结束后，Widget 停留1秒后隐藏

    void setupUI();    // 初始化UI布局
    void resetState(); // 重置Widget状态，隐藏所有金币和清空文本
};

#endif // COINCHANGEWIDGET_H
