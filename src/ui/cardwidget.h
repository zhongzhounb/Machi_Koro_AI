#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout> // 仍然需要包含，尽管不用于主布局
#include <QHBoxLayout> // 仍然需要包含
#include <QFrame>
#include <QGraphicsColorizeEffect>
#include <QPixmap>
#include <QResizeEvent> // <--- 新增：用于处理 resizeEvent

#include "card.h" // 包含 global.h

// 辅助函数：将 Color 枚举转换为 RGB
QString colorToRGB(Color color);
// 辅助函数：将 Color 枚举转换为背景图片路径
QString colorToImagePath(Color color);
// 辅助函数：将卡牌类名转换为建筑图片路径
QString classNameToImagePath(const QString& className);

class CardWidget : public QFrame
{
    Q_OBJECT
public:
    explicit CardWidget(Card* card, QWidget* parent = nullptr);
    ~CardWidget();

    Card* getCard() const { return m_card; }
    void updateCardUI(); // 根据 m_card 数据更新所有 UI 元素

public slots:
    void onCardStateChanged(Card* card, State newState);
    void onCardValueChanged(Card* card);

protected:
    void paintEvent(QPaintEvent *event) override; // 重写 paintEvent
    void resizeEvent(QResizeEvent *event) override; // <--- 新增：重写 resizeEvent

private:
    Card* m_card; // 指向模型中的 Card 对象
    QLabel* m_nameLabel;        // 显示名字 (现在包含类型图标)
    QLabel* m_costLabel;        // 显示花费
    QLabel* m_activationRangeLabel; // 显示激活范围
    QLabel* m_descriptionLabel;     // 显示描述
    QLabel* m_stateOverlayLabel;    // 用于显示“CLOSED”覆盖层

    QLabel* m_img;       // 图片，表示建筑形状的图，覆盖在背景图片之上

    void setupUI();
    void applyCardStyle();
    void updateLabelGeometries(); // <--- 新增：更新所有标签的几何形状
};

#endif // CARDWIDGET_H
