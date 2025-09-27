#ifndef MYROUNDEDVIDEOITEM_H
#define MYROUNDEDVIDEOITEM_H

#include <QGraphicsVideoItem>
#include <QPainter>
#include <QPainterPath>

class MyRoundedVideoItem : public QGraphicsVideoItem
{
public:
    explicit MyRoundedVideoItem(QGraphicsItem* parent = nullptr) : QGraphicsVideoItem(parent) {}

    // 设置圆角半径
    void setBorderRadius(qreal radius) {
        if (m_borderRadius != radius) {
            m_borderRadius = radius;
            update(); // 请求重绘以应用新的圆角
        }
    }

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        // 如果设置了圆角半径，则应用裁剪路径
        if (m_borderRadius > 0) {
            QPainterPath path;
            // boundingRect() 返回当前视频项的矩形区域
            path.addRoundedRect(boundingRect(), m_borderRadius, m_borderRadius);
            painter->setClipPath(path);
        }
        // 调用基类的 paint 方法来渲染视频内容
        QGraphicsVideoItem::paint(painter, option, widget);
    }

private:
    qreal m_borderRadius = 0; // 存储圆角半径
};

#endif // MYROUNDEDVIDEOITEM_H
