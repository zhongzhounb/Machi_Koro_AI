#include "CitySilhouettePainter.h"
#include <QWidget> // 用于parentWidget()
#include <QDebug>
#include <algorithm> // For std::sort

CitySilhouettePainter::CitySilhouettePainter(QObject *parent)
    : QObject(parent)
    , m_backgroundState(GameBackgroundWidget::Dawn) // 初始状态为清晨
{
}

void CitySilhouettePainter::setCityColor(const QColor& color)
{
    if (m_cityColor != color) {
        m_cityColor = color;
        emit cityColorChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

// 背景状态 setter
void CitySilhouettePainter::setBackgroundState(GameBackgroundWidget::BackgroundState state)
{
    if (m_backgroundState != state) {
        m_backgroundState = state;
        emit backgroundStateChanged();
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update();
    }
}

void CitySilhouettePainter::setSize(const QSize& size)
{
    if (m_size != size) { // 只有当尺寸改变时才重新生成路径
        m_size = size;
        generateBuildingFeaturesAndMainPath(); // 尺寸改变时重新生成所有建筑特征和主路径
        if (QWidget* p = qobject_cast<QWidget*>(parent())) p->update(); // 尺寸改变也需要重绘
    }
}

void CitySilhouettePainter::paint(QPainter* painter)
{
    if (m_size.isEmpty()) return;

    painter->setRenderHint(QPainter::Antialiasing); // 启用抗锯齿

    // 1. 绘制主城市剪影
    painter->setPen(Qt::NoPen); // 无边框
    painter->setBrush(m_cityColor); // 填充颜色，使用动画属性
    painter->drawPath(m_mainCitySilhouettePath); // 绘制主城市剪影路径

    // 3. 绘制窗户
    drawWindows(painter);
}

// 添加窗户的方法 (移除了 isDeepNightLit 参数)
void CitySilhouettePainter::addWindow(const QRectF& rect, bool isNightLit)
{
    m_windows.append({rect, isNightLit});
}

// 清除所有窗户
void CitySilhouettePainter::clearWindows()
{
    m_windows.clear();
}

void CitySilhouettePainter::generateBuildingFeaturesAndMainPath()
{
    m_buildingFeatures.clear();
    m_mainCitySilhouettePath = QPainterPath();
    if (m_size.isEmpty()) return;

    qreal w = m_size.width();
    qreal h = m_size.height();

    // 定义城市主剪影的顶部轮廓点（相对Widget尺寸）
    QVector<QPointF> topProfilePointsRel = {
        {0.0, 0.70}, //斜顶房
        {0.10, 0.75},

        {0.10, 0.80},//平顶
        {0.20, 0.80},

        {0.20, 0.71},//平顶屋檐房
        {0.19, 0.71},
        {0.19, 0.70},
        {0.31, 0.70},
        {0.31, 0.71},
        {0.30, 0.71},

        {0.30, 0.80},//三角顶房
        {0.35, 0.75},
        {0.40, 0.80},

        {0.40, 0.85},//信号塔
        {0.42, 0.85},
        {0.44, 0.70},
        {0.43, 0.70},
        {0.42, 0.65},
        {0.445, 0.65},
        {0.45, 0.6},
        {0.455, 0.65},
        {0.48, 0.65},
        {0.47, 0.70},
        {0.46, 0.70},
        {0.48, 0.85},
        {0.50, 0.85},

        {0.50, 0.75},//商业大楼
        {0.60, 0.75},

        {0.60, 0.70},//三角顶房
        {0.65, 0.65},
        {0.70, 0.70},

        {0.70, 0.80},//三角顶房含烟囱
        {0.75, 0.75},
        {0.77,0.77},
        {0.77,0.75},
        {0.78,0.75},
        {0.78,0.78},
        {0.80, 0.80},

        {0.80, 0.85},//工厂，模拟三个斜顶厂房
        {0.833, 0.80},
        {0.833, 0.85},
        {0.866, 0.80},
        {0.866, 0.85},
        {0.90, 0.80},

        {0.90, 0.70},//旗帜平顶房
        {0.92,0.70},
        {0.90,0.68},
        {1.0,0.68},
        {0.98,0.70},
        {1.0, 0.70}

    };

    double window_w=0.025;
    double window_h=0.045;

    //定义窗户 (移除了 isDeepNightLit 参数)
    addWindow(QRectF(0.02, 0.75, window_w, window_h), false);
    addWindow(QRectF(0.02, 0.82, window_w, window_h), false);

    addWindow(QRectF(0.12, 0.82, window_w, window_h), false);
    addWindow(QRectF(0.16, 0.82, window_w, window_h), true);

    addWindow(QRectF(0.22, 0.87, window_w, window_h), true);
    addWindow(QRectF(0.22, 0.73, window_w, window_h), true); // 原本是 true, true
    addWindow(QRectF(0.22, 0.80, window_w, window_h), true);

    addWindow(QRectF(0.32, 0.82, window_w, window_h), true); // 原本是 true, true
    addWindow(QRectF(0.36, 0.82, window_w, window_h), false);

    addWindow(QRectF(0.42, 0.88, window_w, window_h), true);
    addWindow(QRectF(0.46, 0.88, window_w, window_h), true);

    addWindow(QRectF(0.52, 0.85, window_w, window_h), true); // 原本是 true, true
    addWindow(QRectF(0.52, 0.78, window_w, window_h), true);

    addWindow(QRectF(0.62, 0.80, window_w, window_h), true);
    addWindow(QRectF(0.62, 0.87, window_w, window_h), false); // 原本是 false, true
    addWindow(QRectF(0.62, 0.73, window_w, window_h), true);
    addWindow(QRectF(0.66, 0.87, window_w, window_h), true);

    addWindow(QRectF(0.72, 0.82, window_w, window_h), true); // 原本是 true, true
    addWindow(QRectF(0.76, 0.82, window_w, window_h), false);

    addWindow(QRectF(0.82, 0.88, window_w, window_h), true);
    addWindow(QRectF(0.86, 0.88, window_w, window_h), true); // 原本是 true, true

    addWindow(QRectF(0.92, 0.80, window_w, window_h), true);
    addWindow(QRectF(0.92, 0.87, window_w, window_h), true);
    addWindow(QRectF(0.92, 0.73, window_w, window_h), true);
    addWindow(QRectF(0.96, 0.73, window_w, window_h), false);


    // 构建主城市剪影路径
    m_mainCitySilhouettePath.moveTo(0, h); // 从左下角开始
    for (const auto& pRel : topProfilePointsRel) {
        m_mainCitySilhouettePath.lineTo(pRel.x() * w, pRel.y() * h);
    }
    m_mainCitySilhouettePath.lineTo(w, h); // 连接到右下角
    m_mainCitySilhouettePath.closeSubpath(); // 闭合路径

}

void CitySilhouettePainter::drawWindows(QPainter* painter)
{
    QColor windowDayColor = QColor("#C4A78D"); // 白天窗户颜色
    QColor windowLitColor = QColor(255, 255, 150); // 暖黄色灯光

    painter->setPen(Qt::NoPen);

    bool isDaytime = (m_backgroundState == GameBackgroundWidget::Dawn ||
                      m_backgroundState == GameBackgroundWidget::Noon ||
                      m_backgroundState == GameBackgroundWidget::Dusk);

    for (const auto& window : m_windows) {
        QColor currentColor;
        bool shouldDraw = false;

        if (isDaytime) {
            // 白天状态，窗户显示为白色
            currentColor = windowDayColor;
            shouldDraw = true;
        } else if (m_backgroundState == GameBackgroundWidget::Night) {
            // 夜晚状态，根据 isNightLit 判断是否发光
            if (window.isNightLit) {
                currentColor = windowLitColor;
                shouldDraw = true;
            }
            // 如果 isNightLit 为 false，则 shouldDraw 保持 false，窗户不绘制
        }
        // 移除了 GameBackgroundWidget::DeepNight 状态的判断

        if (shouldDraw) {
            painter->setBrush(currentColor);
            // 将相对位置和大小转换为实际像素坐标
            QRectF actualRect(window.rect.x() * m_size.width(),
                              window.rect.y() * m_size.height(),
                              window.rect.width() * m_size.width(),
                              window.rect.height() * m_size.height());
            painter->drawRect(actualRect);
        }
    }
}
