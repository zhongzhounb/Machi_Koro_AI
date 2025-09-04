#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include "global.h"
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QFont>
#include <QResizeEvent>
#include <QTimer> // 新增：QTimer 头文件
#include <QPoint> // 新增：QPoint 头文件

class CoinsWidget;
class Card;
class AutoFitTextLabel;

// 辅助函数声明
QString typeToImg(Type type);
QColor colorToQColor(Color color);
QString colorToImagePath(Color color);
QString classNameToImagePath(const QString& className);
QPixmap QPixmapToRound(const QPixmap & img, int radius);

class CardWidget : public QFrame
{
    Q_OBJECT
public:
    explicit CardWidget(Card* card, ShowType type = ShowType::Ordinary, QWidget* parent = nullptr);
    ~CardWidget();

    Card* getCard() const { return m_card; }
    void setAnimated(bool animated){
        m_isAnimated = animated;
    };

signals:
    void clicked(Card* card); // 鼠标点击时发出卡牌
    void hovered(Card* card); // 鼠标悬停时发出卡牌
    // 新增：请求显示详细卡牌的信号
    void requestShowDetailedCard(Card* card, QPoint globalPos);
    // 新增：请求隐藏详细卡牌的信号
    void requestHideDetailedCard();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override; // 鼠标进入事件
    void leaveEvent(QEvent *event) override;     // 鼠标离开事件
    void mouseMoveEvent(QMouseEvent *event) override; // 新增：鼠标移动事件

private slots:
    void onCardStateChanged(Card* card, State newState);
    void onCardValueChanged(Card* card);
    void onHoverDelayTimeout(); // 新增：悬停计时器超时槽函数

private:
    Card* m_card;
    ShowType m_type;
    double m_aspectRatio = 1.0;
    bool m_isResizing = false; // 添加保护标志以防止递归

    QStackedLayout* m_mainLayout;
    QLabel* m_backgroundImgLabel;
    QLabel* m_imgLabel;
    QWidget* m_textContainer;
    QVBoxLayout* m_textLayout;

    QLabel* m_activationRangeLabel;
    QLabel* m_nameLabel;
    CoinsWidget* m_costLabel;
    QLabel* m_descriptionLabel;
    QLabel* m_stateOverlayLabel;

    QTimer* m_hoverDelayTimer; // 悬停延迟计时器
    QPoint m_lastMousePos;     // 记录鼠标的最后位置

    void initUI();
    void updateData();
    bool m_isAnimated = false;
};

#endif // CARDWIDGET_H
