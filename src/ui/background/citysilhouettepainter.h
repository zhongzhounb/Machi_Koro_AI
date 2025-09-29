#ifndef CITYSILHOUETTEPAINTER_H
#define CITYSILHOUETTEPAINTER_H

#include <QObject>
#include <QPainter>
#include <QColor>
#include <QPainterPath>
#include <QSize>
#include <QVector>
#include <QRectF> // 用于窗户位置和大小

#include "GameBackgroundWidget.h" // 包含主Widget的头文件以访问其枚举

class CitySilhouettePainter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QColor cityColor READ cityColor WRITE setCityColor NOTIFY cityColorChanged)
    // 背景状态属性，用于控制窗户发光
    Q_PROPERTY(GameBackgroundWidget::BackgroundState backgroundState READ backgroundState WRITE setBackgroundState NOTIFY backgroundStateChanged)

public:
    explicit CitySilhouettePainter(QObject *parent = nullptr);

    // Getter方法
    QColor cityColor() const { return m_cityColor; }
    GameBackgroundWidget::BackgroundState backgroundState() const { return m_backgroundState; } // 背景状态 getter

    // Setter方法
    void setCityColor(const QColor& color);
    void setBackgroundState(GameBackgroundWidget::BackgroundState state); // 背景状态 setter

    // 设置绘图区域大小，并重新生成城市路径和建筑特征
    void setSize(const QSize& size);

    // 执行绘图操作
    void paint(QPainter* painter);

    // 添加窗户的方法 (移除了 isDeepNightLit 参数)
    void addWindow(const QRectF& rect, bool isNightLit);
    // 清除所有窗户
    void clearWindows();

signals:
    void cityColorChanged();
    void backgroundStateChanged(); // 背景状态信号

private:
    QSize m_size;          // 绘图区域大小
    QColor m_cityColor;    // 城市剪影颜色（动画属性）
    GameBackgroundWidget::BackgroundState m_backgroundState; // 当前背景状态

    // 窗户结构体 (移除了 isDeepNightLit 成员)
    struct Window {
        QRectF rect;         // 窗户在Widget中的相对位置和大小 (0.0 到 1.0)
        bool isNightLit;     // 夜晚状态是否发光
    };
    QVector<Window> m_windows; // 存储所有窗户

    // 屋顶类型枚举
    enum RoofType { Flat, SlopedLeft, SlopedRight, Triangular };

    // 建筑屋顶特征结构体
    struct BuildingFeature {
        QRectF baseRectRel;           // 相对Widget尺寸的基准矩形 (x, y, width, height)
        // y是屋顶特征的起始Y坐标（即主建筑块的顶部Y）
        // height通常为0，因为屋顶特征是附加在上面的
        RoofType roofType;            // 屋顶类型
        bool hasLightningRod;         // 是否有避雷针
        qreal roofHeightRel;          // 屋顶相对于基准矩形顶部的高度比例
        qreal lightningRodHeightRel;  // 避雷针的高度比例
    };
    QVector<BuildingFeature> m_buildingFeatures; // 存储所有建筑的屋顶特征

    QPainterPath m_mainCitySilhouettePath; // 整体的城市暗色剪影路径

    // 根据当前尺寸生成建筑特征和主剪影路径
    void generateBuildingFeaturesAndMainPath();
    // 绘制窗户
    void drawWindows(QPainter* painter);

};

#endif // CITYSILHOUETTEPAINTER_H
