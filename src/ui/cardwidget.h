#ifndef CARDWIDGET_H
#define CARDWIDGET_H
#include "global.h"
#include <QFrame> // CardWidget 继承 QFrame
#include <QLabel>
#include <QMouseEvent>
#include <QStackedLayout>

class Card;

// 辅助函数声明（保持不变）
QString typeToImg(Type type);
QColor colorToQColor(Color color);
QString colorToImagePath(Color color);
QString classNameToImagePath(const QString& className);
QPixmap QPixmapToRound(const QPixmap & img, int radius); // 引入圆角函数

class CardWidget : public QFrame // 继承 QFrame
{
    Q_OBJECT
public:
    explicit CardWidget(Card* card,ShowType type=ShowType::Ordinary, QWidget* parent = nullptr);
    ~CardWidget();

    Card* getCard() const { return m_card; }

signals:
    void clicked(Card* card);

protected:
    // CardWidget 自身不再需要重写 resizeEvent 来处理比例，
    // 因为 AspectRatioWidget 会处理其内容的比例。
    // 如果你没有其他 QFrame 级别的 resize 逻辑，可以移除此重写。
    // void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onCardStateChanged(Card* card, State newState);
    void onCardValueChanged(Card* card);

private:
    //数据
    Card* m_card;
    //显示模式
    ShowType m_type;

    // 布局 (现在是 m_cardContentContainer 的布局)
    QStackedLayout* m_mainLayout; // 这个布局现在是 m_cardContentContainer 的布局

    // 各层 QLabel 成员保持不变，但它们的父对象将是 m_cardContentContainer
    QLabel* m_backgroundImgLabel; // 1.背景层
    QLabel* m_imgLabel;           // 2.建筑图片层
    QVBoxLayout* m_textLayout;    // 3.文本层 (注意：这是一个布局，它的父对象是 textContainer)
    QWidget* m_textContainer;     // 3.文本层容器，承载文本布局和标签

    QLabel* m_activationRangeLabel; // 3.1激活范围
    QLabel* m_nameLabel;            // 3.2建筑名字
    QLabel* m_costLabel;            // 3.3花费
    QLabel* m_descriptionLabel;               // 3.4描述
    QLabel* m_stateOverlayLabel;              // 4.覆盖层

    void initUI();
    void updateData();
    void updatePosition(); // 可能不再需要，或用于微调
};

#endif // CARDWIDGET_H
