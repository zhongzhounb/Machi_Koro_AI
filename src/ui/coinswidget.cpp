#include "coinswidget.h"


CoinsWidget::CoinsWidget(int coinsNum,QWidget* parent)
    :QWidget(parent),
    m_coinsNum(coinsNum),
    m_mainLayout(new QStackedLayout(this)),
    m_coinsIconLabel(new QLabel(this)),
    m_textLabel(new QLabel(this)){
    //全显示
    m_mainLayout->setStackingMode(QStackedLayout::StackAll);
    //当前文字居中
    m_coinsIconLabel->setAlignment(Qt::AlignCenter);
    m_textLabel->setAlignment(Qt::AlignCenter);
    m_coinsIconLabel->setText("🪙");
    setCoinsNum(m_coinsNum);

    m_mainLayout->addWidget(m_textLabel);
    m_mainLayout->addWidget(m_coinsIconLabel);

    setLayout(m_mainLayout);
};
CoinsWidget::~CoinsWidget(){};

void CoinsWidget::setCoinsNum(int coinsNum){
    m_coinsNum=coinsNum;
    m_textLabel->setText(QString("%1").arg(m_coinsNum));
}

// 重写 resizeEvent 以自我调整尺寸和宽高比
void CoinsWidget::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);

    const QSize allocatedSize = event->size();
    int width = allocatedSize.width();
    int height = allocatedSize.height();

    int fontSize = qMax(5, height/3);
    int iconSize = qMax(5, height/2);
    m_textLabel->setFont(QFont("YouYuan", fontSize, QFont::Bold));
    m_coinsIconLabel->setFont(QFont("YouYuan",iconSize, QFont::Bold));
};
