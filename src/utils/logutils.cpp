#include "logutils.h"

QString LogUtils::formatMutiStealCoinsLog(const QMap<int, QList<QString>>& coinsToPlayers){
    QString log;
    bool firstFlag=false;
    for (QMap<int, QList<QString>>::const_iterator it = coinsToPlayers.constBegin(); it != coinsToPlayers.constEnd(); ++it) {
        //标记分割符号
        if(firstFlag)
            log+="，";
        firstFlag=true;
        //计算多少钱
        int coins = it.key();
        //前缀
        if(coins>0)
            log+="偷了";
        else
            log+="没偷到";
        bool secondFlag=false;
        for (const QString& playerName : it.value()){
            //分割号
            if(secondFlag)
                log+="、";
            secondFlag=true;
            //名称
            log+=playerName;
        }
        //金币记录
        if(coins>0)
            log+=QString(" %1 金币").arg(coins);
        else
            log+="的金币";
    }
    return log;
}
