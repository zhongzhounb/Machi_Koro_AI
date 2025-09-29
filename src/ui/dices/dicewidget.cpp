#include "dicewidget.h"
#include <QResizeEvent>
#include <QDebug>

DiceWidget::DiceWidget(QWidget* parent, int diceNum)
    : QWidget(parent),
    m_diceNum(0),
    m_aspectRatio(1.0f)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    for (int i = 1; i <= 6; i++)
    {
        // 1. 加载原始高质量图片
        QPixmap originalPixmap(QString(":/resources/images/dice/%1.png").arg(i));
        if (originalPixmap.isNull()) {
            qDebug() << "骰子贴图错误！";
            continue;
        }

        QPixmap croppedPixmap = originalPixmap.copy(QRect(180, 320, 600, 600));

        m_originalPixmaps.append(croppedPixmap); // *** 关键点1: 存储原始图 ***

        // 2. 创建 QLabel 并设置它的父对象
        QLabel* imgLabel = new QLabel(this);
        imgLabel->setScaledContents(true);
        imgLabel->hide();
        m_imgLabels.append(imgLabel);
    }

    setDiceNum(diceNum);
}

DiceWidget::~DiceWidget() {}

void DiceWidget::setDiceNum(int diceNum)
{
    if (diceNum < 1 || diceNum > 6 || diceNum == m_diceNum) {
        return;
    }

    if (m_diceNum > 0) {
        m_imgLabels[m_diceNum - 1]->hide();
    }

    m_diceNum = diceNum;

    if (m_diceNum > 0) {
        m_imgLabels[m_diceNum - 1]->show();
    }

    // *** 关键点2: 切换图片后，立即更新它的尺寸和显示的图像 ***
    updateLabelGeometry();
}

void DiceWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); // 先调用父类的实现
    updateLabelGeometry();
}

// 这是核心的图像处理函数
void DiceWidget::updateLabelGeometry()
{
    // 如果没有骰子面或者当前面无效，则不执行任何操作
    if (m_imgLabels.isEmpty() || m_diceNum <= 0) {
        return;
    }

    // 1. 计算居中、保持宽高比的矩形区域
    QRect parentRect = this->rect();
    float currentRatio = static_cast<float>(parentRect.width()) / parentRect.height();
    QRect childRect;

    if (currentRatio > m_aspectRatio) {
        int childHeight = parentRect.height();
        int childWidth = static_cast<int>(childHeight * m_aspectRatio);
        childRect.setRect((parentRect.width() - childWidth) / 2, 0, childWidth, childHeight);
    } else {
        int childWidth = parentRect.width();
        int childHeight = static_cast<int>(childWidth / m_aspectRatio);
        childRect.setRect(0, (parentRect.height() - childHeight) / 2, childWidth, childHeight);
    }

    // 如果计算出的尺寸为0，则不进行操作，避免 QPixmap::scaled 警告
    if (childRect.width() <= 0 || childRect.height() <= 0) {
        return;
    }

    // 2. 获取当前应该显示的 QLabel 和它的原始 Pixmap
    QLabel* currentLabel = m_imgLabels[m_diceNum - 1];
    const QPixmap& originalPixmap = m_originalPixmaps[m_diceNum - 1];

    // 3. 将 QLabel 移动到计算好的位置
    currentLabel->setGeometry(childRect);

    // 4. *** 关键点3: 永远从原始高质量图进行缩放 ***
    currentLabel->setPixmap(originalPixmap.scaled(
        childRect.size(), // 缩放到目标矩形的大小
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation // 使用平滑变换以获得更好的质量
        ));
}
