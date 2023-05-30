// ###### Config options ################


// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

using namespace Asedio;

void asignarValoresDefensas(std::vector<int>& valores, std::vector<Defense*>& defensas, const std::list<Defense*>& defenses)
{
    for(std::list<Defense*>::const_iterator pos = defenses.begin(); pos != defenses.end(); pos++)
    {
        valores.push_back(2 * ((*pos)->range - (*pos)->damage - 2*(*pos)->dispersion) + 1.7 * (*pos)->damage - 0.2*(*pos)->cost);
        defensas.push_back(*pos);
    }
}

void rellenarTSR(std::vector<std::vector<int> >& TSP, const std::vector<int>& valores, const std::vector<Defense*>& defensas)
{
    for(int i = 0; i < TSP[0].size(); ++i)
    {
        if(i < defensas[0]->cost)
            TSP[0][i] = 0;
        else 
            TSP[0][i] = valores[0];
    }
    for(int i = 1; i < defensas.size(); ++i)
        for(int j = 0; j < TSP[0].size(); ++j)
        {
            if(j < defensas[i]->cost)
                TSP[i][j] = TSP[i-1][j];
            else
                TSP[i][j] = std::max(TSP[i-1][j], TSP[i-1][j-defensas[i]->cost] + valores[i]);
        }
}

void escogidos(const std::vector<std::vector<int> >& TSP, std::list<int>& selectedIDs,
     const std::vector<Defense*>& defensas, int ases)
{
    int i = defensas.size()-1;
    while(i > 0)
    {
        if(TSP[i][ases] != TSP[i-1][ases])
        {
            selectedIDs.push_back(defensas[i]->id);
            ases -= defensas[i]->cost;
        }
        i--;
    } 
    if(TSP[0][ases] != 0)
    {
        selectedIDs.push_back(defensas[i]->id);
        ases -= defensas[i]->cost;
    }
}

void DEF_LIB_EXPORTED selectDefenses(std::list<Defense*> defenses, unsigned int ases, std::list<int>& selectedIDs
            , float mapWidth, float mapHeight, std::list<Object*> obstacles) 
{
    std::vector<int> valoresDefensas;
    std::vector<Defense*> defensas;

    //incluyo centro de extraccion de minerales
    std::list<Defense*>::iterator centroExtraccion = defenses.begin();
    selectedIDs.push_back((*centroExtraccion)->id);
    ases -= (*centroExtraccion)->cost;

    //creo tabla de subproblemas resueltos
    std::vector<std::vector<int> > TSP(defenses.size()-1, std::vector<int>(ases+1)); 
    //asigno valores a las defensas
    asignarValoresDefensas(valoresDefensas, defensas, defenses);
    //elimino la primera defensa (centro de extraccion de minerales; ya esta incluida)
    valoresDefensas.erase(valoresDefensas.begin());
    defensas.erase(defensas.begin());
    //relleno TSP -> algoritmo mochila
    rellenarTSR(TSP, valoresDefensas, defensas);
    //recupero defensas
    escogidos(TSP, selectedIDs, defensas, ases);
    
}
