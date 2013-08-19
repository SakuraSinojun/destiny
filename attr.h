
#pragma once

#include <string>

class Attr
{
public:
    Attr();
    Attr(int init);
    virtual ~Attr();
    int GetAttr(std::string attr);

    int Str() { return str_; }
    int Dex() { return dex_; }
    int Con() { return con_; }
    int Wis() { return wis_; }
    int Int() { return int_; }
    int Chr() { return chr_; }

    int str_;
    int dex_;
    int con_;
    int wis_;
    int int_;
    int chr_;
};


