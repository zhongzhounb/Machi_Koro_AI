#include "gamecommand.h"
#include "gamestate.h" // 需要访问 GameState 来查找 Player*
#include "player.h"    // 需要 Player 类的定义
#include "card.h"
// 初始化静态成员变量
int GameCommand::s_commandId=0;

GameCommand::GameCommand(CommandType type,Player* sourcePlayer, QObject* parent,
                         QList<Card*> cards,Player* activePlayer)
    : m_type(type),QObject(parent),  m_sourcePlayer(sourcePlayer),m_cards(cards),m_activePlayer(activePlayer){
    //自增id，用以回放
    m_id=s_commandId++;

    if(!m_cards.empty())
        m_card=m_cards.last();

    /*创建命令自动计算优先级*/
    //1.首先排命令优先级
    m_priority=type*10000;
    //2.命令相同则排相同名称卡
    if(!m_card){
        m_priority+=m_card->getNameId()*100;
        //3.相同名称卡只有红卡有执行顺序，优先执行上家
        if(m_card->getColor()==Color::Red)
            m_priority+=(sourcePlayer->getId()-activePlayer->getId()+MAX_PLAYER_NUM)%MAX_PLAYER_NUM;
    }

}

PromptData GameCommand::getPromptData(GameState* state) const{
    PromptData pd;//之后可能要补充卡牌动画
    return pd;
};

bool GameCommand::setInput(int optionId,GameState* state,GameController* controller){
    execute(state,controller);
    return true;
};


