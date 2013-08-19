

#include "item.h"
#include "dice.h"

Item::Item()
{
    const char * _table[] = {
        "猫之灵敏帽子",
        "聪慧之闪避裤子",
        "熊之强壮戒指",
        "灵狐贪婪闪避项链",
        "聪慧板甲",
        "夜视灵狐强壮鞋子",
        "猫之幸运聪慧发卡",
        "幸运聪慧戒指",
    };

    int s = sizeof(_table) / sizeof(_table[0]);
    int d = dice(s, 1) - 1;
    name = _table[d];
    enable = false;
    type = (ItemType)(dice(ITEM_MAX - 1, 1) - 1);
}

Item::~Item()
{
}

int Item::value(void)
{
    return 0;
}

std::string Item::desc(void)
{
    return "";
}

void Item::Enable(bool b)
{
    enable = b;
}




