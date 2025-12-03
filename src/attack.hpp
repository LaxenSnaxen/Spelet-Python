#pragma once

#include <string>

class attack 
{
    public:
        attack(int id, const std::string& name, int baseDamage, int staminaCost, std::string primaryStat, std::string flavourText) : id(id), name(name), damage(baseDamage), staminaCost(staminaCost), primaryStat(primaryStat), flavourText(flavourText) {}
        
        int getId() const { return id; }
        std::string getName() const { return name; }
        int getDamage() const { return damage; }
        int getStaminaCost() const { return staminaCost; }
        std::string getPrimaryStat() const { return primaryStat; }
        std::string getFlavourText() const { return flavourText; }
        
    private:
        int id;
        std::string name;
        int damage;
        int staminaCost;
        std::string primaryStat;
        std::string flavourText;
};