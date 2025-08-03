#ifndef SHOPPINGMALL_H
#define SHOPPINGMALL_H
#include "card.h"

class ShoppingMall: public Card
{
    Q_OBJECT
public:
    explicit ShoppingMall(QObject* parent=nullptr);
    ~ShoppingMall(){};
    QString getDescription() const override;
};

#endif // SHOPPINGMALL_H
