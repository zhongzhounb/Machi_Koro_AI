#ifndef CITYSILHOUETTEPAINTER_H
#define CITYSILHOUETTEPAINTER_H

#include <QObject>
#include <QPainter>
#include <QColor>
#include <QPainterPath>
#include <QSize>

class CitySilhouettePainter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QColor cityColor READ cityColor WRITE setCityColor NOTIFY cityColorChanged)

public:
    explicit CitySilhouettePainter(QObject *parent = nullptr);

    // Getter方法
    QColor cityColor() const { return m_cityColor; }

    // Setter方法
    void setCityColor(const QColor& color);

    // 设置绘图区域大小，并重新生成城市路径
    void setSize(const QSize& size);

    // 执行绘图操作
    void paint(QPainter* painter);

signals:
    void cityColorChanged();

private:
    QSize m_size;          // 绘图区域大小
    QColor m_cityColor;    // 城市剪影颜色（动画属性）
    QPainterPath m_cityPath; // 预计算的城市剪影路径

    // 根据当前尺寸创建城市剪影路径
    void createCityPath();
};

#endif // CITYSILHOUETTEPAINTER_H
