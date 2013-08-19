
#pragma once

#include <string>

class Item {
public:
    Item();
    ~Item();

    Item(const Item& o) {
        type = o.type;
        name = o.name;
        enable = false;
    }

    Item& operator=(const Item& o) {
        type = o.type;
        name = o.name;
        enable = false;
        return *this;
    }

    typedef enum {
        ITEM_HEAD = 0,
        ITEM_TORSO,
        ITEM_LEG,
        ITEM_FOOT,
        ITEM_NECK,
        ITEM_FINGER,
        ITEM_HAND,
        ITEM_FOOD,
        ITEM_POTION,
        ITEM_SCROLL,
        ITEM_BOOK,
        ITEM_OTHER,
        ITEM_MAX
    } ItemType;

    ItemType    type;
    std::string name;
    void        Enable(bool b);
    bool        Enabled(void) { return enable; }

    int value(void);
    std::string desc(void);

private:
    bool        enable;
};



