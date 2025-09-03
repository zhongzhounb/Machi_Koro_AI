#ifndef CARDWIDGET_H
#define CARDWIDGET_H

#include "global.h"
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QFont>
#include <QResizeEvent> // 需要包含此头文件

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
    void clicked(Card* card);

protected:
    // 重写 resizeEvent 以自我调整尺寸和宽高比
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onCardStateChanged(Card* card, State newState);
    void onCardValueChanged(Card* card);

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

    void initUI();
    void updateData();
    bool m_isAnimated = false;
};

#endif // CARDWIDGET_H
