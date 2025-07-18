#include "investtechnologycompanycommand.h"
#include "player.h"
#include "card.h"
#include "gamestate.h"
#include "gamecontroller.h"

InvestTechnologyCompanyCommand::InvestTechnologyCompanyCommand(Player* sourcePlayer, Card* card, QObject* parent, bool isFailed, const QString& failureMessage)
    : GameCommand(CommandType::InvestTechnologyCompany, sourcePlayer,parent,card,nullptr,isFailed,failureMessage){
}

PromptData GiveCardCommand::getPromptData(GameState* state) {
    PromptData pt;
    switch (m_currentStep){
    case 1:{//选择卡阶段
        pt.type=PromptData::SelectCard;
        pt.promptMessage=QString("请选择一张你需要给出的卡牌");
        for(QList<Card*> cards:m_sourcePlayer->getCards()){
            Card* card=cards.last();
            if(card->getType()==Type::Landmark)
                pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能将地标建筑给予其他玩家。"});
            if(card->getType()==Type::Office)
                pt.options.append(OptionData{card->getId(),card->getName(),0,"你不能将紫色建筑给予其他玩家。"});
            else
                pt.options.append(OptionData{card->getId(),card->getName(),1,""});
        }
        return pt;
    }
    case 2:{//选择玩家阶段
        pt.type=PromptData::SelectPlayer;
        pt.promptMessage=QString("请选择将%1赠予的玩家").arg(state->getCard(m_userInput[0])->getName());
        for(Player* player:state->getPlayers())
            if(player!=m_sourcePlayer)
                pt.options.append(OptionData{player->getId(),player->getName(),1,""});

        return pt;
    }
    case 3:{//确认阶段
        pt.type=PromptData::Popup;
        pt.promptMessage=QString("确认要将%1给予给%2吗？").arg(state->getCard(m_userInput[0])->getName()).arg(state->getPlayer(m_userInput[1])->getName());
        pt.options.append(OptionData{1,"确定",1,""});
        pt.options.append(OptionData{0,"重新选择",1,""});
        return pt;
    }
    }

    return pt;
}
// 获取默认选项（无选项时禁止调用）
int GiveCardCommand::getAutoInput( const PromptData& promptData ,GameState* state) {
    switch (m_currentStep){
    case 1:{//选择卡阶段，默认选本卡
        return m_card->getId();
    }
    case 2:{//选择玩家阶段，随机选择玩家
        int opId=promptData.options[RandomUtils::instance().generateInt(0,promptData.options.size()-1)].id;
        m_randIndex.append(1);//保存随机数下标
        return opId;
    }
    case 3:{//选择玩家阶段
        return 1;
    }
    }
    return 1;

};
// 设置选项，返回是否要继续获得选项（无选项时禁止调用）
bool GiveCardCommand::setInput(int optionId,GameState* state) {
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

void InvestTechnologyCompanyCommand::execute(GameState* state, GameController* controller) {
    if(m_sourcePlayer->getCoins()==0){
        m_isFailed=true;
        m_failureMessage="没钱投资了。";
        return ;
    }

    //读取选项
    QVariantList chooes=m_userChoice.value("valueList").toList();
    int value=chooes.indexOf(0);

    //不投资，不产生日志
    if(m_sourcePlayer->getCoins()==0){
        m_isFailed=true;
        m_failureMessage="选择不投资。";
        return;
    }

    //投资
    m_sourcePlayer->delCoins(1);
    m_card->changeValue(1);
    m_coinsSum=m_card->getValue();
}

QString InvestTechnologyCompanyCommand::getLog()const {
    QString log=QString("【%1】 %2 ").arg(m_card->getName()).arg(m_sourcePlayer->getName());

    if(m_isFailed)
        log+=QString("%1。").arg(m_failureMessage);
    else
        log+=QString("投资了 1 金币 ，目前共投资了 %1 金币！").arg(m_coinsSum);

    return log;
}

