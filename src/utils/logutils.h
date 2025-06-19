#ifndef LOGUTILS_H
#define LOGUTILS_H

#include "global.h"

namespace LogUtils {
    QString formatMutiStealCoinsLog(const QMap<int, QList<QString>>& coinsToPlayers);
}

#endif // LOGUTILS_H
