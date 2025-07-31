#include "dicewidget.h"
#include<QTimer.h>
DiceWidget::DiceWidget(QWidget* parent,int diceNum)
    :QWidget(parent),
    m_diceNum(diceNum),
    m_mainLayout(new QStackedLayout(this)){

    m_mainLayout->setContentsMargins(0,0,0,0);
    setContentsMargins(0,0,0,0);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    for(int i=1;i<=6;i++)
    {
        QLabel* imgLabel=new QLabel(this);
        QPixmap dicePixmap(QString(":/resources/images/dice/%1.png").arg(i));
        imgLabel->setScaledContents(true);
        imgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 建议：让 QLabel 更灵活地填充可用空间
        if (!dicePixmap.isNull()) {
            imgLabel->setPixmap(dicePixmap.copy(QRect(40,70,160,160)));
            m_mainLayout->addWidget(imgLabel);
            m_imgLabels.append(imgLabel);
        }
        else
            qDebug()<<"骰子贴图错误！";
    }
    //setStyleSheet("QWidget {background: red;}");

    setDiceNum(m_diceNum);

    setLayout(m_mainLayout);
    QTimer::singleShot(0, this, [this](){
        if (!m_imgLabels.isEmpty()) {
            qDebug() << "Initial DiceWidget size:" << size();
            qDebug() << "Initial Current QLabel size:" << m_imgLabels.at(m_mainLayout->currentIndex())->size();
        }
    });
};
DiceWidget::~DiceWidget(){};

void DiceWidget::setDiceNum(int diceNum){
    if(diceNum==0)
        return;
    m_diceNum=diceNum;
    m_mainLayout->setCurrentIndex(m_diceNum-1);
    // 切换骰子图片后，立即更新当前 QLabel 的几何形状，以防万一
    if (!m_imgLabels.isEmpty()) {
        QLabel* currentLabel = m_imgLabels.at(m_mainLayout->currentIndex());
        if (currentLabel) {
            currentLabel->setGeometry(rect()); // 让 QLabel 填充 DiceWidget 的整个区域
        }
    }

}

// 重写 resizeEvent 以自我调整尺寸和宽高比
/*void DiceWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (m_isResizing) {
        return;
    }
    m_isResizing = true;

    const QSize allocatedSize = event->size();
    int width = allocatedSize.width();
    int height = allocatedSize.height();

    // 假设 m_aspectRatio 已经正确初始化
    // 例如：m_aspectRatio = 宽度 / 高度;
    if (m_aspectRatio == 0) { // 避免除以零
        m_isResizing = false;
        return;
    }

    if (static_cast<double>(width) / m_aspectRatio < height) { // 使用 double 进行浮点除法
        height = static_cast<int>(static_cast<double>(width) / m_aspectRatio);
    } else {
        width = static_cast<int>(static_cast<double>(height) * m_aspectRatio);
    }

    QRect newGeometry(0, 0, width, height);


    // 修正后的代码
    newGeometry.moveCenter(QRect(QPoint(), allocatedSize).center());

    // 使用 setGeometry 来调整大小和位置，而不是 setFixedSize
    setGeometry(newGeometry);

    if (!m_imgLabels.isEmpty()) {
        QLabel* currentLabel = m_imgLabels.at(m_mainLayout->currentIndex());
        if (currentLabel) {
            // 让 QLabel 填充 DiceWidget 的整个客户区
            currentLabel->setGeometry(0, 0, width, height); // width 和 height 是 DiceWidget 的新尺寸
        }
    }

    //m_mainLayout->activate();

    m_isResizing = false;
}*/
