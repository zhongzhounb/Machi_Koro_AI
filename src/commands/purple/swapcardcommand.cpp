#include "swapcardcommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"
#include "randomutils.h"
SwapCardCommand::SwapCardCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::SwapCard, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

PromptData SwapCardCommand::getPromptData(GameState* state) {
    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择卡阶段
        pt.type=PromptData::SelectCard;
        pt.promptMessage=QString("请选择一张你需要交换出去的卡牌");
        for(QList<Card*> cards:m_sourcePlayer->getCards()){
            Card* card=cards.last();
            if(card->getType()==Type::Landmark)
                pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能交换地标建筑。"});
            else if(card->getType()==Type::Office)
                pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能交换紫色建筑。"});
            else
                pt.options.append(OptionData{card->getId(),card->getName(),1,""});
        }
        return pt;
    }
    case 2:{//选择阶段
        pt.type=PromptData::SelectCard;
        pt.promptMessage=QString("请选择一张你需要交换回来的卡牌");
        for(Player* player:state->getPlayers())
            if(player!=m_sourcePlayer)
                for(QList<Card*> cards:player->getCards()){
                    Card* card=cards.last();
                    if(card->getType()==Type::Landmark)
                        pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能交换地标建筑。"});
                    else if(card->getType()==Type::Office)
                        pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能交换紫色建筑。"});
                    else
                        pt.options.append(OptionData{card->getId(),card->getName(),1,""});
                }
        return pt;
    }
    case 3:{//确认阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("确认要将【%1】与【%2】交换吗？").arg(state->getCard(m_userInput[0])->getName()).arg(state->getCard(m_userInput[1])->getName());
        pt.options.append(OptionData{1,"确定",1,""});
        pt.options.append(OptionData{0,"重新选择",1,""});
        return pt;
    }
    }

    return pt;
}
// 获取默认选项（无选项时禁止调用）
int SwapCardCommand::getAutoInput( const PromptData& promptData ,GameState* state) {
    switch (m_currentStep){
    case 1:{//选个费用最小的
        int minn=999;
        int opId=-1;
        for(OptionData option:promptData.options)
            if(option.state==1){
                int cost=state->getCard(option.id)->getCost();
                if(state->getCard(option.id)->getCost()<minn){
                    minn=cost;
                    opId=option.id;
                }
            }
        if(opId==-1)
            qDebug()<<"没有找到任何换出的卡牌";
        return opId;
    }
    case 2:{//选个费用最多的
        int maxn=-1;
        int opId=-1;
        for(OptionData option:promptData.options)
            if(option.state==1){
                int cost=state->getCard(option.id)->getCost();
                if(state->getCard(option.id)->getCost()>maxn){
                    maxn=cost;
                    opId=option.id;
                }
            }
        if(opId==-1)
            qDebug()<<"没有找到任何换入的卡牌";
        return opId;
    }
    case 3:{//确认阶段
        return 1;
    }
    }
    return 1;

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool SwapCardCommand::setInput(int optionId,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择卡阶段
        m_userInput.append(optionId);
        m_currentStep=2;
        return false;
    }
    case 2:{//选择玩家阶段
        m_userInput.append(optionId);
        m_currentStep=3;
        return false;
    }
    case 3:{//选择玩家阶段
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

void SwapCardCommand::execute(GameState* state, GameController* controller) {
    //读取选项
    int cardId1=m_userInput[0];
    int cardId2=m_userInput[1];
    Card* card1=nullptr, *card2=nullptr;
    Player* player2=nullptr;
    //找这两张卡在哪
    for(Player* player:state->getPlayers())
        for(QList<Card*>cards:player->getCards())
            if(cards.last()->getId()==cardId1)
                card1=cards.last();
            else if(cards.last()->getId()==cardId2){
                card2=cards.last();
                player2=player;
            }
    qDebug()<<card1;
    qDebug()<<card2;
    qDebug()<<player2;
    //日志记录
    m_cardName1=card1->getName();
    m_cardName2=card2->getName();
    m_playerName=player2->getName();
    //操作
    m_sourcePlayer->addCard(card2);
    player2->addCard(card1);
    m_sourcePlayer->delCard(card1);
    player2->delCard(card2);
}

QString SwapCardCommand::getLog()const {
    QString log=QString("【%1】 %2 ").arg(m_card->getName()).arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else
        log+=QString("用【%1】交换了 %2 的【%3】。").arg(m_cardName1).arg(m_playerName).arg(m_cardName2);

    return log;
}
