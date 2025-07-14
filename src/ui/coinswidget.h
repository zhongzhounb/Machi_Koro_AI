#ifndef COINSWIDGET_H
#define COINSWIDGET_H

#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QStackedLayout>
#include <QFont>
#include <QResizeEvent> // 需要包含此头文件
class CoinsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CoinsWidget( QWidget* parent = nullptr,int coinsNum=0);
    ~CoinsWidget();

public slots:
    void setCoinsNum(int coinsNum);

protected:
    // 重写 resizeEvent 以自我调整尺寸和宽高比
    void resizeEvent(QResizeEvent *event) override;

private:
    int m_coinsNum;
    QStackedLayout* m_mainLayout;
    QLabel* m_coinsIconLabel;
    QLabel* m_textLabel;
};

#endif // COINSWIDGET_H
