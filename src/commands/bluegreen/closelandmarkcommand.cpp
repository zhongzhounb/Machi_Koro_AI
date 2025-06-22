#include "closelandmarkcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

CloseLandmarkCommand::CloseLandmarkCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::CloseLandmark, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

// 检查是否需要用户交互（可选交互：如果自己的地标都不够拆，直接全拆了，不用选择）
bool CloseLandmarkCommand::requiresUserInput()const {
    if(m_sourcePlayer->getCardNum(m_card->getName(),State::Opening)>=m_sourcePlayer->getTypeCardNum(Type::Landmark,State::Opening))
        return false;
    return true;
};

void CloseLandmarkCommand::execute(GameState* state, GameController* controller) {
    //计算有多少卡牌
    m_cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    //目前没这个方法
    if(m_isFailed)
        return;
    //计算总共有多少地标
    m_landmarkNum=m_sourcePlayer->getTypeCardNum(Type::Landmark,State::Opening);
    //读取选项
    QVariantList cardIds;
    //如果能拆完（包含了没有地标建筑的情况）则无需交互
    if(m_cardNum>=m_landmarkNum){
        for(QList<Card*> cards:m_sourcePlayer->getCards())
            if(cards.first()->getType()==Type::Landmark)
                cardIds.append(cards.first()->getId());
    }
    else//如果不能拆完，则需要交互选择
        cardIds = m_userChoice.value("valueList").toList();
    //关闭地标建筑
    for(QList<Card*> cards:m_sourcePlayer->getCards())
        for (const QVariant& cardId : cardIds)
            if(cards.first()->getId()==cardId.toInt()){
                m_sourcePlayer->setCardState(cards.first(),State::Closing);
                m_closeNames.append(cards.first()->getName());
            }
    //计算收益
    m_coinsSum=m_closeNames.size()*m_card->getValue();
    //赚钱
    m_sourcePlayer->addCoins(m_coinsSum);
}

QString CloseLandmarkCommand::getLog()const {
    QString log=QString("【%1】%2 %3 ").arg(m_card->getName())
                      .arg(m_cardNum==1?"":QString("*%1").arg(m_cardNum))
                      .arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else if(m_closeNames.size()==0)
        log+=QString("没有拆除任何建筑，也没有获得任何金币。");
    else{
        log+="拆除了";
        for(QString name:m_closeNames)
            log+=QString("【%1】，").arg(name);
        log+=QString("获得 %1 金币。").arg(m_coinsSum);
    }

    return log;
}
