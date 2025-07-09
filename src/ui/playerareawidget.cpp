#include "playerareawidget.h"
#include "card.h"
#include "slotwidget.h"
#include "cardwidget.h"

PlayerAreaWidget::PlayerAreaWidget(Player* player,bool isHBoxLayout,bool isLandMark,QWidget* parent): QWidget(parent)
    ,m_player(player),m_isHBoxLayout(isHBoxLayout),m_isLandMark(isLandMark){
    if(isHBoxLayout)
        m_mainLayout=new QHBoxLayout(this);
    else
        m_mainLayout=new QVBoxLayout(this);


    m_mainLayout->addStretch();
    m_mainLayout->setAlignment(Qt::AlignCenter);
    setLayout(m_mainLayout);

}

PlayerAreaWidget::~PlayerAreaWidget(){

}

int getOrderId(Card* card){
    int num=0;
    //先排左值
    num+=card->getActLNum()*100000;
    //后排右值
    num+=card->getActRNum()*1000;
    //再排颜色
    num+=(int)card->getColor()*100;
    //最后防重
    num+=card->getNameId();
    return num;
}

void PlayerAreaWidget::onCardAdded(Player* player,Card* card){
    if(player!=m_player)
        return;

    //地标和普通卡分类
    if(m_isLandMark&&card->getType()!=Type::Landmark||!m_isLandMark&&card->getType()==Type::Landmark)
        return;

    //如果有相同的卡，直接放在槽内
    for(SlotWidget* slotWidget:m_slots){
        CardWidget* cardWidget=slotWidget->topCard();
        if(!cardWidget)
            qDebug()<<"PlayerAreaWidget错误：找到个无卡空槽";
        if(cardWidget->getCard()->getName()==card->getName()){
            slotWidget->pushCard(new CardWidget(card,ShowType::Ordinary,this));
            return;
        }
    }

    //没找到，则新建一个
    int addIndex=0;
    for(SlotWidget* slotWidget:m_slots){
        CardWidget* cardWidget=slotWidget->topCard();
        if(!cardWidget)
            qDebug()<<"PlayerAreaWidget错误：找到个无卡空槽";
        if(getOrderId(cardWidget->getCard())>getOrderId(card))
            return;
        addIndex++;
    }
    //插入位置
    SlotWidget* newSlotWidget=new SlotWidget(false,Color::BackNone,this);
    newSlotWidget->pushCard(new CardWidget(card,ShowType::Ordinary,this));
    m_slots.insert(addIndex,newSlotWidget);
    m_mainLayout->insertWidget(addIndex,newSlotWidget);

}
