#include "ai.h"
#include "gamestate.h"
#include "player.h"
#include "card.h"
AI:: AI(QObject* parent ): QObject(parent){}

double prob(int diceNum,int pointNum){
    if(diceNum==1){
        if(pointNum>=1&&pointNum<=6)
            return 1.0/6.0;
    }else if(diceNum==2){
        if(pointNum>=2&&pointNum<=12)
            return (6.0-fabs(7.0-pointNum))/36.0;
    }
    return 0.0;
}

double AI::getIncome(Card* card,Player* owner,Player* activePlayer,GameState* state,int count){
    int combo=card->getComboNum(owner,activePlayer,state);
    int val=card->getValue();
    int num=owner->getCardNum(card->getName(),State::Opening);
    if(count!=-1)
        combo=count;
    return combo*val*num;
}

double AI::simulate(Player* owner,int pointNum,GameState* state){
    //拷贝金币
    QMap<Player*,int>coins;
    QList<Player*>players=state->getPlayers();
    for(Player* player:players)
        coins[player]=player->getCoins();
    //先执行红卡（todo：用n表示可能有问题，因为是1开始编号的）
    int n=players.size();
    for(int i=1;i<n;i++){
        int id=(owner->getId()-i+n)%n;
        Player* player=state->getPlayer(id);
        for(QList<Card*> cards:player->getCards()){
            Card* card=cards.last();
            if(card->getColor()==Color::Red&&card->isActivate(player,owner,pointNum)){
                int count=getIncome(card,player,owner,state);
                coins[owner]-=count;
                coins[player]+=count;
            }
        }
    }
    //再执行蓝卡
    for(Player* player:players)
        for(QList<Card*> cards:player->getCards()){
            Card* card=cards.last();
            if(card->getColor()==Color::Blue&&card->isActivate(player,nullptr,pointNum)){
                int count=getIncome(card,player,nullptr,state);
                coins[player]+=count;
            }
        }
    //再执行绿卡
    for(QList<Card*> cards:owner->getCards()){
        Card* card=cards.last();
        if(card->getColor()==Color::Green&&card->isActivate(owner,nullptr,pointNum)){
            if(card->getName()=="拆迁公司"){//拆迁公司收益减去上回合卡牌最小值
                int count=getIncome(card,owner,nullptr,state);
                Data data=m_data.value(owner);
                coins[owner]+=count-data.lastCardMinValue;
            }
            else if(card->getName()=="搬家公司"){//搬家公司减去所拆的值再减去回合收益（可负）（因为拆地标一定要建回来，但是拆卡牌不一定要补回来，相当于浪费一回合）
                int num=owner->getCardNum(card->getName(),State::Opening);
                int count=0;
                for(QList<Card*> cards:owner->getCards()){
                    Card* landmark=cards.last();
                    if(landmark->getType()==Type::Landmark&&landmark->getState()==State::Opening&&num>0){
                        count+=card->getValue()-landmark->getCost()-m_roundValue;
                        num--;
                    }
                }
                coins[owner]+=count;
            }
            else{
                int count=getIncome(card,owner,nullptr,state);
                coins[owner]+=count;
            }
        }
    }
    //再执行紫卡
    for(QList<Card*> cards:owner->getCards()){
        Card* card=cards.last();
        if(card->getColor()==Color::Purple&&card->isActivate(owner,nullptr,pointNum)){
            if(card->getName()=="商业中心"){
                double otherMax=0.0;
                for(Player* player:players)
                    if(player!=owner){
                        Data otherData=m_data.value(player);
                        otherMax=qMax(otherMax,otherData.lastCardMaxValue);
                    }

                Data data=m_data.value(owner);
                coins[owner]+=otherMax-data.lastCardMinValue;
            }
            else {//剩下的都是aoe收钱的，所以可以合在一起计算
                for(Player* player:players)
                    if(player!=owner){
                        int count=0;
                        if(card->getName()=="税务所")
                            count=player->getCoins()>=10?player->getCoins()/2:0;
                        else if(card->getName()=="出版社")
                            count=player->getTypeCardNum(Type::Store,State::None)+player->getTypeCardNum(Type::Restaurant,State::None);
                        else
                            count=card->getValue();

                        count=qMin(count,player->getCoins());
                        coins[player]-=count;
                        coins[owner]+=count;
                    }
            }
        }
    }
    return coins[owner];
}

void AI::update(GameState* state){
    QList<Player*>players=state->getPlayers();

    //模拟一遍更新值
    for(Player* player:players)
        for(int i=1;i<=14;i++)
            m_data[player].value[i]=simulate(player,i,state);


    //再做计算
    for(Player* player:players){
        Data data=m_data.value(player);

        bool hasHarbor=player->getCardNum("港口",State::Opening);
        bool hasAmuseMentPark=player->getCardNum("游乐园",State::Opening);
        bool hasRadioTower=player->getCardNum("广播塔",State::Opening);

        //计算期望（含【港口】）
        for(int i=1;i<=12;i++){
            double val=data.value[i];
            if(hasHarbor&&i>=10)//【港口】如果有港口则和+2的比大小
                val=qMax(val,data.value[i+2]);
            data.OneDiceEx+=data.value[i]*prob(1,i);
            data.TwoDiceEx+=data.value[i]*prob(2,i);
        }

        //【游乐园】如果有游乐园，期望收益多1/6，因为抛两个有1/6概率进入新的一回合（既然本回合两个的收益更大了，那下一回合应该还是两个的收益大
        if(hasAmuseMentPark)
            data.TwoDiceEx+=data.TwoDiceEx/6.0;

        //计算重抛前概率
        bool isRollTwo=data.OneDiceEx<data.TwoDiceEx;
        for(int i=1;i<=12;i++)
            if(!isRollTwo)
                data.prob[i]=prob(1,i);
            else
                data.prob[i]=prob(2,i);

        //【广播塔】在收益低于期望值重抛，如果抛出游乐园，视为多一个回合，收益为value+roundvalue，为负说明：亏损的钱不够多一个回合补回，就重抛
        if(hasRadioTower){
            double maxDiceEx=qMax(data.OneDiceEx,data.TwoDiceEx);
            double reRollProbSum=0.0;
            for(int i=1;i<=12;i++)
                if(data.value[i]<maxDiceEx){//如果小于期望值
                    double reRollProb=data.prob[i];
                    if(isRollTwo&&hasRadioTower&&i%2==0&&data.value[i]+m_roundValue>=0){//是抛两个、有游乐园、偶数点、正收益
                        reRollProb-=1.0/36.0;
                        data.roundEx+=1.0/36.0;//增加玩家的回合数期望
                    }
                    reRollProbSum+=reRollProb;
                    data.prob[i]-=reRollProb;
                }
            //重抛，概率补回
            for(int i=1;i<=12;i++)
                if(!isRollTwo)
                    data.prob[i]+=reRollProbSum*prob(1,i);
                else
                    data.prob[i]+=reRollProbSum*prob(2,i);
        }

        //+2概率补回
        if(hasHarbor)
            for(int i=12;i>=10;i--)
                if(data.value[i+2]>data.value[i]){
                    data.prob[i+2]=data.prob[i];
                    data.prob[i]=0.0;
                }
    }
}

//获取应该投掷的骰子数（比较两个DiceEx）
int AI::getRollDiceNum(Player* player){

    Data data=m_data.value(player);
    bool hasStation=player->getCardNum("火车站",State::Opening);

    if(hasStation&&data.OneDiceEx<data.TwoDiceEx)
        return 2;
    return 1;
}

//获取重抛的骰子数（0为保持)（用value比较DiceEx，游乐园则用value+roundValue>=0）
int AI::getReRollDiceNum(Player* player,int num1,int num2){

    Data data=m_data.value(player);
    bool hasAmuseMentPark=player->getCardNum("游乐园",State::Opening);
    int rollDiceNum=getRollDiceNum(player);
    int diceEx=rollDiceNum==1?data.OneDiceEx:data.TwoDiceEx;
    int sum=num1+num2;

    if(data.value[sum]<diceEx){
        if(hasAmuseMentPark&&num1==num2&&data.value[sum]+m_roundValue>0)//【游乐园】正收益特判
            return 0;
        return rollDiceNum;
    }
    //维持
    return 0;
}

//获取是否要+2（比较value）
int AI::getAddDiceNum(Player* player,int sum){

    Data data=m_data.value(player);
    if(data.value[sum]<data.value[sum+2])
        return 1;
    return 0;
}

int comboNum(Player* player,QList<QString>cardNames){
    int num=0;
    for(QString name:cardNames)
        num+=player->getCardNum(name,State::None);
    return num;
}


double AI::comboEx(Player* owner,QString name,GameState* state){
    double sum=0.0;
    Data data=m_data.value(owner);
    for(QList<Card*>cards:owner->getCards()){
        Card* card=cards.last();
        if(card->getName()==name){
            for(int pointNum=card->getActLNum();pointNum<=card->getActRNum();pointNum++)
                sum+=getIncome(card,owner,nullptr,state,1)*data.prob[pointNum];
        }
    }
    return sum;
}

//单张卡价值
double AI::getCardRecentEx(Card* card,Player* owner,GameState*state){

    //处理landmark
    if(card->getName()=="广播塔")
        return 999;
    else if(card->getName()=="机场")
        return 888;
    else if(card->getName()=="游乐园")
        return 777;
    else if(card->getName()=="购物中心")
        return 666;
    else if(card->getName()=="火车站"){
        Data data=m_data.value(owner);
        if(data.OneDiceEx<data.TwoDiceEx||owner->getTypeCardNum(Type::Landmark,State::Opening)>=4)
            return 555;
        else
            return m_roundValue;
    }
    else if(card->getName()=="港口"){
        if(comboNum(owner,{"寿司店","鲭鱼船","金枪鱼船","拆迁公司"})>0||owner->getTypeCardNum(Type::Landmark,State::Opening)>=4)
            return 444;
        else
            return m_roundValue;
    }

    double recentEx=0.0;
    for(int pointNum=card->getActLNum();pointNum<=card->getActRNum();pointNum++){
        //绿卡
        if(card->getColor()==Color::Green){
            Data data=m_data.value(owner);
            double val;
            if(card->getName()=="拆迁公司"){
                int minn=999;
                for(QList<Card*> cards:owner->getCards())
                    if(cards.last()->getType()==Type::Landmark)
                        minn=qMin(minn,cards.last()->getCost());
                val=card->getValue()-minn-m_roundValue;
            }
            else if(card->getName()=="搬家公司")
                val=card->getValue()-data.lastCardMinValue;
            else
                val=card->getComboNum(owner,nullptr,state)*card->getValue();
            recentEx=val*data.prob[pointNum];
        }//蓝卡
        else if(card->getColor()==Color::Blue){
            //单次卡收益
            for(Player* player:state->getPlayers()){
                Data data=m_data.value(player);
                recentEx+=card->getValue()*data.prob[pointNum];
            }
            //组合卡收益
            if(card->getType()==Type::Agriculture)
                recentEx+=comboEx(owner,"果蔬超市",state);
            if(card->getType()==Type::Husbandry)
                recentEx+=comboEx(owner,"奶酪工厂",state);
            if(card->getType()==Type::Industry)
                recentEx+=comboEx(owner,"家具工厂",state);
            if(card->getName()=="花田")
                recentEx+=comboEx(owner,"花店",state);
        }//红卡
        else if(card->getColor()==Color::Red){
            for(Player* player:state->getPlayers())//假设是当前的人投掷到
                if(player!=owner){
                    Data data=m_data.value(player);
                    int coins=0;
                    for(Player* player2:state->getPlayers(player,true))//假设他的卡
                        if(player2!=player){
                            QList<Card*> cards=player2->getCardsForName(card->getName());
                            if(!cards.empty())
                                coins+=getIncome(cards.last(),player2,player,state);//已计算不能收的情况
                            if(player2==owner)//到自己收完钱停止计算
                                break;
                        }
                    recentEx+=qMin(card->getValue(),qMax(player->getCoins()-coins,0))*data.prob[pointNum];
                }
        }
        else if(card->getColor()==Color::Purple){
            QList<Player*>players=state->getPlayers();
            int coins=0;
            if(card->getName()=="商业中心"){
                double otherMax=0.0;
                for(Player* player:players)
                    if(player!=owner){
                        Data otherData=m_data.value(player);
                        otherMax=qMax(otherMax,otherData.lastCardMaxValue);
                    }

                Data data=m_data.value(owner);
                coins+=otherMax-data.lastCardMinValue;
            }
            else {//剩下的都是aoe收钱的，所以可以合在一起计算
                for(Player* player:players)
                    if(player!=owner){
                        int count=0;
                        if(card->getName()=="税务所")
                            count=player->getCoins()>=10?player->getCoins()/2:0;
                        else if(card->getName()=="出版社")
                            count=player->getTypeCardNum(Type::Store,State::None)+player->getTypeCardNum(Type::Restaurant,State::None);
                        else
                            count=card->getValue();

                        count=qMin(count,player->getCoins());
                        coins+=count;
                    }
            }
            Data data=m_data.value(owner);
            recentEx+=coins*data.prob[pointNum];
        }
    }
    return recentEx;
}

int AI::getBuyCardId(PromptData pd,Player* player,GameState* state){
    //先找出所有可买卡牌
    QList<Card*>cards;
    for(OptionData op:pd.options)
        if(op.state==1)
            cards.append(state->getCard(op.id));

    double maxn=0.0;
    int opId=0;
    for(Card* card:cards){
        double val=getCardRecentEx(card,player,state);
        if(val>maxn){
            maxn=val;
            opId=card->getId();
        }
    }

    return 0;
}
