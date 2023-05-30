// ###### Config options ################
// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include "cronometro.h"
#include <algorithm>
using namespace Asedio;       

struct celda
{
    double valor;
    int row, col;
    celda() : valor(0), row(0), col(0) {}
    celda(int row, int col, double valor) : row(row), col(col), valor(valor) {}
    bool operator<(const celda& c) const
    {
        return valor < c.valor;
    }
    bool operator<=(const celda& c) const
    {
        return valor < c.valor || valor == c.valor; 
    }
};

Vector3 cellToPosition(int i, int j, float cellWidth, float cellHeight)
{ 
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0); 
}

void positionToCell(const Vector3 pos, int &i_out, int &j_out, float cellWidth, float cellHeight)
{ 
    i_out = (int)(pos.y * 1.0f/cellHeight); 
    j_out = (int)(pos.x * 1.0f/cellWidth); 
}  

float defaultCellValue(int row, int col, bool** freeCells, int nCellsWidth, int nCellsHeight
    , float mapWidth, float mapHeight, List<Object*> obstacles, List<Defense*> defenses) {
    	
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;

    Vector3 cellPosition((col * cellWidth) + cellWidth * 0.5f, (row * cellHeight) + cellHeight * 0.5f, 0);
    	
    float val = 0;
    for (List<Object*>::iterator it=obstacles.begin(); it != obstacles.end(); ++it) {
	    val += _distance(cellPosition, (*it)->position);
    }

    return val;
}


bool factible(int row, int col, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight, std::list<Object*> obstacles, 
        std::list<Defense*> defenses, Defense* currentDefense)
{
    Vector3 defensePosition = cellToPosition(row, col, mapWidth/nCellsWidth, mapHeight/nCellsHeight);
    float defenseRatio = currentDefense->radio;
    //no se sale del mapa 
    if(defensePosition.y + defenseRatio > mapHeight || defensePosition.x + defenseRatio > mapWidth || 
        defensePosition.x - defenseRatio < 0 || defensePosition.y - defenseRatio < 0)
        return false;

    //no choca con obstaculos
    for(std::list<Object*>::iterator currentObstacle = obstacles.begin(); currentObstacle != obstacles.end(); currentObstacle++) 
        if(_distance(defensePosition, (*currentObstacle)->position) < (defenseRatio + (*currentObstacle)->radio))
            return false;

    //no choca con las anteriores defensas
    for(std::list<Defense*>::iterator defense = defenses.begin(); *defense != currentDefense; defense++) 
    {
        if(_distance(defensePosition, (*defense)->position) < defenseRatio + (*defense)->radio)
            return false;
    }
    //si llega hasta aqui es porque la posicion es valida
    return true;
}


void extraeCeldaSO(std::vector<celda>& lista, celda& c)
{
    std::vector<celda>::iterator lugar;
    for(std::vector<celda>::iterator pos = lista.begin(); pos != lista.end(); pos++)
    {
        if(c.valor < pos->valor)
        {
            c = *pos;
            lugar = pos;
        }
    }
    lista.erase(lugar);
}


void rellenaVector(bool** freeCells, std::vector<celda>& v, int nCellsWidth, int nCellsHeight,
            std::list<Object*> obstacles, std::list<Defense*> defenses, float mapWidth, float mapHeight)
{
    for(int i = 0; i < nCellsHeight; i++)
        for(int j = 0; j < nCellsWidth; j++)
            v.push_back(celda(i, j, defaultCellValue(i, j, freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses)));
}

bool sortDefenses(Defense* d1, Defense* d2)
{
    return (d1->attacksPerSecond + d1->damage + d1->dispersion) > (d2->attacksPerSecond + d2->damage + d2->dispersion);
}

void insertionSort(std::vector<celda>& v, int i, int j) 
{
    for(int pos = i; pos < j; ++pos)
    {
        int aux = pos;
        celda x = v[aux];
        while(aux != i && x < v[aux - 1]) 
        {
            v[aux] = v[aux - 1];
            --aux;
        }
        v[aux] = x;
    }
}

int pivote(std::vector<celda>& v, int i, int j)
{
    int p = i;
    celda x = v[i];
    for(int k = i+1; k < j && (p < j - 1); k++)
    {
        if(v[k] <= x)
        {
            p++;
            std::swap(v[p], v[k]);
        }
    }
    v[i] = v[p];
    v[p] = x;
    return p;
}
void fusion(std::vector<celda>& v, int i, int k, int j)
{
    int n = j - i;
    int p = i, q = k;
    std::vector<celda> w(v.size());
    for(int l = 0; l < n; l++)
    {
        if(p < k && (q >= j || v[p] <= v[q]))
        {
            w[l] = v[p];
            p++;
        }
        else
        {
            w[l] = v[q];
            q++;
        }
    }

    for(int l = 0; l < n; l++)
        v[i + l] =  w[l];
}

void ordenacion_fusion(std::vector<celda>& v, int i, int j)
{
    int n = j - i;
    if(n <= 3)
        insertionSort(v, i, j);
    else
    {
        int k = i + n/2;
        ordenacion_fusion(v, i, k);
        ordenacion_fusion(v, k, j);
        fusion(v, i, k, j); 
    }
}

void ordenacion_rapida(std::vector<celda>& v, int i, int j)
{
    int n = j - i;
    if(n <= 3)
        insertionSort(v, i, j);
    else
    {
        int p = pivote(v, i, j);
        ordenacion_rapida(v, i, p);
        ordenacion_rapida(v, p+1, j);
    }
}

void DEF_LIB_EXPORTED placeDefenses_SO(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) 
{
    std::vector<celda> vCeldas;
    rellenaVector(freeCells, vCeldas, nCellsWidth, nCellsHeight, obstacles, defenses, mapWidth, mapHeight);
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
    std::list<Defense*>::iterator currentDefense = defenses.begin();
    while(currentDefense != defenses.end() && !vCeldas.empty()) 
    {
        celda mejorPosicion;
        extraeCeldaSO(vCeldas, mejorPosicion);
        if(factible(mejorPosicion.row, mejorPosicion.col, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, *currentDefense))
        {
            //std::cout << mejorPosicion.valor << " ";
            (*currentDefense)->position = cellToPosition(mejorPosicion.row, mejorPosicion.col, cellWidth, cellHeight);
            ++currentDefense;
        }
    }
    //std::cout << std::endl;
}

void DEF_LIB_EXPORTED placeDefenses_F(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) 
{
    std::vector<celda> vCeldas;
    rellenaVector(freeCells, vCeldas, nCellsWidth, nCellsHeight, obstacles, defenses, mapWidth, mapHeight);
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
    std::list<Defense*>::iterator currentDefense = defenses.begin();
    int inicio = (vCeldas.size() * 50) /100;
    ordenacion_fusion(vCeldas, 0, vCeldas.size());
    while(currentDefense != defenses.end() && !vCeldas.empty()) 
    {
        celda mejorPosicion = vCeldas.back();
        if(factible(mejorPosicion.row, mejorPosicion.col, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, *currentDefense))
        {
            //std::cout << mejorPosicion.valor << " ";
            (*currentDefense)->position = cellToPosition(mejorPosicion.row, mejorPosicion.col, cellWidth, cellHeight);
            ++currentDefense;
        }
        vCeldas.pop_back();
    }
    //std::cout << std::endl;
}

void DEF_LIB_EXPORTED placeDefenses_OR(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) 
{
    std::vector<celda> vCeldas;
    rellenaVector(freeCells, vCeldas, nCellsWidth, nCellsHeight, obstacles, defenses, mapWidth, mapHeight);
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    int inicio = vCeldas.size()*0.7;
    insertionSort(vCeldas, inicio, vCeldas.size());
    std::list<Defense*>::iterator currentDefense = defenses.begin();
    while(currentDefense != defenses.end() && !vCeldas.empty())
    {
        celda mejorPosicion = vCeldas.back();
        if(factible(mejorPosicion.row, mejorPosicion.col, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, *currentDefense))
        {
            //std::cout << mejorPosicion.valor << " ";
            (*currentDefense)->position = cellToPosition(mejorPosicion.row, mejorPosicion.col, cellWidth, cellHeight);
            ++currentDefense;
        }
        vCeldas.pop_back();
    }
    //std::cout << std::endl;
}

void DEF_LIB_EXPORTED placeDefenses_M(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) 
{
    std::vector<celda> vCeldas;
    std::vector<std::vector<celda> > mapa(nCellsHeight, std::vector<celda>(nCellsWidth));
    rellenaVector(freeCells, vCeldas, nCellsWidth, nCellsHeight, obstacles, defenses, mapWidth, mapHeight);
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight;
    std::make_heap(vCeldas.begin(), vCeldas.end());
    std::list<Defense*>::iterator currentDefense = defenses.begin();
    while(currentDefense != defenses.end() && !vCeldas.empty())
    {
        std::pop_heap(vCeldas.begin(), vCeldas.end());
        celda mejorPosicion = vCeldas.back();
        if(factible(mejorPosicion.row, mejorPosicion.col, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, *currentDefense))
        {
            //std::cout << mejorPosicion.valor << " ";
            (*currentDefense)->position = cellToPosition(mejorPosicion.row, mejorPosicion.col, cellWidth, cellHeight);
            ++currentDefense;
        }
        vCeldas.pop_back();
        
    }
    //std::cout << std::endl;
}


void DEF_LIB_EXPORTED placeDefenses3(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight
              , List<Object*> obstacles, List<Defense*> defenses) {

    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
    cronometro SO;
    long int r1 = 0;
    const double e_abs = 0.01, e_rel = 0.001;
    SO.activar();
    do {
        placeDefenses_SO(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
	++r1;
    } while(SO.tiempo() < e_abs / e_rel + e_abs);
    SO.parar();

    cronometro OR;
    long int r2 = 0;
    OR.activar();
    do {
        placeDefenses_OR(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
	++r2;
    } while(OR.tiempo() < e_abs / e_rel + e_abs);
    OR.parar();

    cronometro F;
    long int r3 = 0;
    F.activar();
    do {
        placeDefenses_F(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
	    ++r3;
    } while(F.tiempo() < e_abs / e_rel + e_abs);
    F.parar();
    
    cronometro M;
    long int r4 = 0;
    M.activar();
    do {
        placeDefenses_M(freeCells, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses);
	    ++r4;
    } while(M.tiempo() < e_abs / e_rel + e_abs);
    M.parar();

    /*std::cout << "Sin Preordenacion: " << (double)SO.tiempo() / r1 << std::endl;
    std::cout << "Ordenacion rapida: " << (double)OR.tiempo() / r2 << std::endl;*/
    //std::cout << "Fusion: " << (double)F.tiempo() / r3 << std::endl;
    //std::cout << "Monticulo: " << (double)M.tiempo() << std::endl;
    //Sin preordenacion - OF - OR - OM
    //4 - 3 - 2 - 1
    std::cout << (nCellsWidth * nCellsHeight) << "\t" << SO.tiempo() / r1 << "\t" << F.tiempo() / r3 << "\t" << OR.tiempo() / r2 << "\t" << M.tiempo() / r4 << std::endl;
}
