// ###### Config options ################


// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

using namespace Asedio;

void DEF_LIB_EXPORTED selectDefenses(std::list<Defense*> defenses, unsigned int ases, std::list<int> &selectedIDs
            , float mapWidth, float mapHeight, std::list<Object*> obstacles) {
    std::list<Defense*>::iterator centroExtraccion = defenses.begin();
    selectedIDs.push_back((*centroExtraccion)->id);
    ases -= (*centroExtraccion)->cost;
    unsigned int cost = 0;
    std::list<Defense*>::iterator it = defenses.begin();
    while(it != defenses.end()) {
        if(cost + (*it)->cost <= ases) {
            selectedIDs.push_back((*it)->id);
            cost += (*it)->cost;
        }
        ++it;
    }
}
