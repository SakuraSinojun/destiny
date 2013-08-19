

#include "item.h"
#include "dice.h"

Item::Item()
{
    const char * _table[] = {
        "è֮����ñ��",
        "�ϻ�֮���ܿ���",
        "��֮ǿ׳��ָ",
        "���̰����������",
        "�ϻ۰��",
        "ҹ�����ǿ׳Ь��",
        "è֮���˴ϻ۷���",
        "���˴ϻ۽�ָ",
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




