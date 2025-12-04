#include <map>
#include "item.hpp"

class Inventory {
    public:
        void addItem(const item& newItem) {
            int id = newItem.getId();
            if (items.find(id) != items.end()) {
                // Om föremål redan existerar, öka antalet.
                int currentQuantity = items[id].getQuantity();
                items[id] = item(id, newItem.getValue(), currentQuantity + newItem.getQuantity(), newItem.getName(), newItem.getDescription(), newItem.getType());
            } else {
                // Lägg till nytt föremål
                items[id] = newItem;
            }
        }

        void removeItem(int id, int quantity) {
            auto thing = items.find(id);
            if (thing != items.end())
            { 
                {
                    int currentQuantity = thing->second.getQuantity();
                    if (currentQuantity <= quantity) 
                    {
                        items.erase(thing );
                    } 
                    else 
                    {
                        items[id] = item(id, thing->second.getValue(), currentQuantity - quantity, thing->second.getName(), thing->second.getDescription(), thing->second.getType());
                    }
                }
            }
        }

        item* getItem(int id)
        {
            auto article = items.find(id);
            if (article != items.end()) {
                return &article->second;
            }
            return nullptr;
        }

    private:
        std::map<int, item> items;
};