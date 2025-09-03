#ifndef COINSWIDGET_H
#define COINSWIDGET_H

#include <QWidget>
#include <QStackedLayout>
#include <QLabel>
#include <QFont>
#include <QResizeEvent>
#include <QVariantAnimation>

class CoinsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CoinsWidget(QWidget* parent = nullptr, int coinsNum = 0);
    ~CoinsWidget();

public slots:
    void setCoinsNum(int coinsNum);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // 槽函数，用于接收动画的中间值并更新显示
    void updateAnimatedCoins(const QVariant& value);

private:
    int m_targetCoinsNum; // 存储目标金币数量
    QStackedLayout* m_mainLayout;
    QLabel* m_coinsIconLabel;
    QLabel* m_textLabel;
    QVariantAnimation* m_animation; // 动画对象
    bool m_firstSetDone = false;    // 新增标志位：标记是否已经完成首次设置
};

#endif // COINSWIDGET_H
