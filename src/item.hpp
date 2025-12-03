#pragma once

#include <string>
#include <map>
#include "attack.hpp"

class item 
{
    public:
        item(int id, int value, int quantity, const std::string& name, const std::string& description, const std::string& type) : id(id), value(value), quantity(quantity), name(name), description(description), type(type){}
        int getId() const { return id; }
        int getValue() const { return value; }
        int getQuantity() const { return quantity; }
        std::string getName() const { return name; }
        std::string getDescription() const { return description; }
        std::string getType() const { return type; }
        
    private:
        int id;
        int value;
        int quantity = 1;
        std::string name;
        std::string description;
        std::string type;
};

class consumable : public item 
{
    // Ärver från item. Kollar om buffen är permanent, då kommer spelarens egenskaper att permanent ökas. Annars är det en engångs 'grej', då den helar eller återhämtar stamina.
    // Om overtime > 0, så appliceras effekten över tid istället för direkt. Tid räknas i omgångar eller rundor.
    // ENDAST HÄLSA OCH STAMINNA OM EJ PERMANENT!
    public:
        consumable(int id, int value, int quantity, const std::string& name, const std::string& description, const std::string& type, int statRestore, int stat, bool permanent, int overTime = 0) 
        : item(id, value, quantity, name, description, type), statRestore(statRestore), stat(stat), permanent(permanent), overTime(overTime) {}
        
        int getStatRestore() const { return statRestore; }
        int getStat() const { return stat; }
        bool isPermanent() const { return permanent; }
        int getOverTime() const { return overTime; }
    private:
        int stat;
        int statRestore;
        bool permanent;
        int overTime;
};

class equipment : public item 
{
    // Ärver från item. Utrustning som kan ge olika buffs till spelarens egenskaper.
    // Stat egenskaper: Hälsa, Styrka, Kvickhet, Försvar, Utthålighet, Vishet, Intelligens, Karisma, Tur
    public:
        equipment(
        int id, int value, int quantity, const std::string& name, const std::string& description, const std::string& type, 
        int healthBoost, int strengthBoost, int agilityBoost, int defenceBoost, int enduranceBoost, 
        int wisdomBoost, int intelligenceBoost, int charismaBoost, int luckBoost) 
        : item(id, value, quantity, name, description, type),
        healthBoost(healthBoost), strengthBoost(strengthBoost), agilityBoost(agilityBoost), defenceBoost(defenceBoost), enduranceBoost(enduranceBoost),
        wisdomBoost(wisdomBoost), intelligenceBoost(intelligenceBoost), charismaBoost(charismaBoost), luckBoost(luckBoost) {}
        
        int getHealthBoost() const { return healthBoost; }
        int getStrengthBoost() const { return strengthBoost; }
        int getAgilityBoost() const { return agilityBoost; }
        int getDefenceBoost() const { return defenceBoost; }
        int getEnduranceBoost() const { return enduranceBoost; }
        int getWisdomBoost() const { return wisdomBoost; }
        int getIntelligenceBoost() const { return intelligenceBoost; }
        int getCharismaBoost() const { return charismaBoost; }
        int getLuckBoost() const { return luckBoost; }

        
    private:
        int healthBoost;
        int strengthBoost;
        int agilityBoost;
        int defenceBoost;
        int enduranceBoost;
        int wisdomBoost;
        int intelligenceBoost;
        int charismaBoost;
        int luckBoost;
};

class weapon : public equipment 
{
    // Få lägga till en klass som hanterar hur attacker fungerar, varje vapen ska tillföra en attack till spelarens moveset.
    // Varje gång spelaren rustar sig med ett vapen så får dem alternativet att lägga till vapnets signatur attack i deras moveset.
    // Även om dem inte väljer attacken så får dem fortfarande vapnets övriga egenskaper. 
    public:
        weapon(int id, int value, int quantity, const std::string& name, const std::string& description, const std::string& type, 
        int healthBoost, int strengthBoost, int agilityBoost, int defenceBoost, int enduranceBoost, 
        int wisdomBoost, int intelligenceBoost, int charismaBoost, int luckBoost, int handsRequired, const attack& weaponAttack)
        : equipment(id, value, quantity, name, description, type,
                    healthBoost, strengthBoost, agilityBoost, defenceBoost, enduranceBoost,
                    wisdomBoost, intelligenceBoost, charismaBoost, luckBoost), handsRequired(handsRequired), weaponAttack(weaponAttack) {}
    private:
        int handsRequired;
        attack weaponAttack;    
};