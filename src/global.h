#ifndef GLOBAL_H
#define GLOBAL_H

#include <QObject>
#include<Qvector>
#include<QString>
#include <QVariant>
#include <QDebug>
#include <algorithm>
#include <QList>
#include <QMap>
#include <QStack>
#include <QRandomGenerator>

enum class Color{ Blue, Green, Red, Purple,Grey};
enum class Type{ Agriculture, Husbandry, Industry, Fishery, Store, Factory, Company, Restaurant, Office,Landmark};
enum class State { Opening, Closing };

enum class AIRank {
    None,       // 人类玩家
    Easy,       // 简单AI
    Medium,     // 中等AI
    Hard        // 困难AI
};

#endif // GLOBAL_H


