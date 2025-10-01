#ifndef COINCHANGEWIDGET_H
#define COINCHANGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGridLayout> // 尽管我们主要用QStackedLayout，但保留以防万一
#include <QTimer>
#include <QFont>
#include <QVector> // 不再需要QVector<QVector<QLabel*>>
#include <functional> // for std::function
// #include <QPropertyAnimation> // REMOVED: No longer animating geometry directly
#include <QStackedLayout> // For overlaying labels

class Player;

class CoinChangeWidget : public QWidget {
    Q_OBJECT

public:
    explicit CoinChangeWidget(Player* player, QWidget* gameMainWidget, QWidget* parent = nullptr);
    ~CoinChangeWidget();

    void setCoinChangePosFunction(std::function<QPoint(int, int)> func) { m_coinChangePosFunc = func; }

public slots:
    void showChange(Player* player,int amount,int change);

private slots:
    void updateNumberAnimation();
    void hideWidget();
    void flash2xCoinImage(); // ADDED: To show 2x coin image temporarily
    void hide2xCoinImage();  // ADDED: To hide 2x coin image after 35ms

private:
    QStackedLayout* m_stackedLayout;
    QLabel* amountLabel;
    QLabel* coinImageLabel_1_5x; // ADDED: 1.5x coin image (bottom layer)
    QLabel* coinImageLabel_2x;   // ADDED: 2x coin image (middle layer)

    QTimer* animationTimer; // Controls number animation
    QTimer* hideTimer;      // Controls Widget hiding
    QTimer* flashTimer;     // ADDED: Timer to control 2x coin flash duration

    int currentChangeCoins;
    int animationCurrentValue;
    int animationTargetValue;

    Player* m_player;
    QWidget* m_gameMainWidget;
    std::function<QPoint(int, int)> m_coinChangePosFunc;

    // Constants
    const int BASE_AMOUNT_FONT_SIZE = 12;
    const int COIN_1_5X_SIZE_MULTIPLIER = 6; // 1.5倍金币图像的字体大小是基础数字字体大小的倍数
    const int COIN_2X_SIZE_MULTIPLIER = 8;   // 2倍金币图像的字体大小是基础数字字体大小的倍数
    const int TEXT_SIZE_MULTIPLIER = 4;  // 字体倍率
    const int ANIMATION_STEP_MS = 200;
    const int FLASH_DURATION_MS = 100;        // 2倍金币图像闪现的持续时间
    const int HIDE_DELAY_MS = 1500;


    void setupUI();
    void resetState();

    // Helper for calculating sizes
    QSize getWidgetFixedSize() const; // ADDED: Widget的固定大小将基于2倍金币图像
};

#endif // COINCHANGEWIDGET_H
