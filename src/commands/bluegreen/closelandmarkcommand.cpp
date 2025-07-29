#include "closelandmarkcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

CloseLandmarkCommand::CloseLandmarkCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::CloseLandmark, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

//拆最小花费的地标
PromptData CloseLandmarkCommand::getPromptData(GameState* state) {
    // 检查是否需要用户交互（可选交互：如果自己的地标都不够拆，直接全拆了，不用选择）
    int cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);
    int landMarkNum=m_sourcePlayer->getTypeCardNum(Type::Landmark,State::Opening);

    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择阶段
        if(cardNum>=landMarkNum)
            return pt;
        pt.type=PromptData::SelectCard;
        pt.promptMessage=QString("请选择你需要拆除的地标建筑(%1/%2)").arg(m_userInput.size()).arg(cardNum);
        //计算需要拆除的建筑
        QList<Card*>landMarks;
        for(QList<Card*> cards:m_sourcePlayer->getCards())
            if(cards.last()->getType()==Type::Landmark&&cards.last()->getState()==State::Opening)
                landMarks.append(cards.last());
        //设置选项
        for(Card* card:landMarks)
            if(m_userInput.contains(card->getId()))//如果选择了，则高亮
                pt.options.append(OptionData{card->getId(),card->getName(),2,""});
            else//如果没选，则闪烁可选
                pt.options.append(OptionData{card->getId(),card->getName(),1,""});
        return pt;
    }
    case 2:{//确认阶段
        QList<Card*>landMarks;
        for(int cardId:m_userInput)
            landMarks.append(state->getCard(cardId));
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("确认要将%1").arg(landMarks[0]->getName());
        for(int i=1;i<m_userInput.size();i++)
            pt.promptMessage+=QString("、%1").arg(landMarks[i]->getName());
        pt.promptMessage+="拆除吗？";
        pt.options.append(OptionData{1,"确定",1,""});
        pt.options.append(OptionData{0,"重新选择",1,""});
        return pt;
    }
    }

    return pt;
};
// 获取默认选项（无选项时禁止调用）
int CloseLandmarkCommand::getAutoInput( const PromptData& promptData ,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择阶段
        int minn=999;
        int opId=0;
        for(OptionData op:promptData.options)
            if(op.state==1)
            {
                int cost=state->getCard(op.id)->getCost();
                if(minn>cost){
                    minn=cost;
                    opId=op.id;
                }
            }
        return opId;
    }
    case 2:{//确认阶段
        return 1;
    }
    }

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool CloseLandmarkCommand::setInput(int optionId,GameState* state) {
    int cardNum=m_sourcePlayer->getCardNum(m_card->getName(),State::Opening);

    switch (m_currentStep){
    case 1:{//选择阶段
        //增加/减少选项
        if(m_userInput.contains(optionId))//如果包括选择，则说明取消了选择
            m_userInput.removeOne(optionId);
        else//否则是选择
            m_userInput.append(optionId);

        //如果选完了，就进入下一步
        if(m_userInput.size()==cardNum)
            m_currentStep=2;

        return false;
    }
    case 2:{//确认阶段
        //确认则执行完毕
        if(optionId==1)
            return true;

        //否则重新选择
        m_userInput.clear();
        m_currentStep=1;
        return false;
    } 
    }
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
    //如果能拆完（包含了没有地标建筑的情况）则无需交互，自动补全
    if(m_cardNum>=m_landmarkNum){
        for(QList<Card*> cards:m_sourcePlayer->getCards())
            if(cards.last()->getType()==Type::Landmark&&cards.last()->getState()==State::Opening)
                m_userInput.append(cards.last()->getId());
    }
    //关闭地标建筑
    for(QList<Card*> cards:m_sourcePlayer->getCards())
        for (const QVariant& cardId : m_userInput)
            if(cards.last()->getId()==cardId.toInt()){
                cards.last()->setState(State::Closing);
                //关闭变回
                for(QList<Card*>cards2:m_sourcePlayer->getCards()){
                    for(Card* card2:cards2)
                        if(card2->getType()==Type::Store||card2->getType()==Type::Restaurant)
                            card2->changeValue(-1);
                }
                m_closeNames.append(cards.last()->getName());
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
