// ###### Config options ################

//#define PRINT_DEFENSE_STRATEGY 1    // generate map images

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"
#include <math.h>
#include <random>
#include <climits>
#include <iomanip>
#ifdef PRINT_DEFENSE_STRATEGY
#include "ppm.h"

#endif

#ifdef CUSTOM_RAND_GENERATOR
RAND_TYPE SimpleRandomGenerator::a;
#endif

using namespace Asedio;
struct celda
{
    double valor;
    int row, col;
    celda() : valor(0), row(0), col(0) {}
    celda(double valor, int row, int col) : valor(valor), row(row), col(col) {}
    bool operator<(const celda& c) const
    {
        return valor < c.valor;
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

celda extraeCelda(std::list<celda>& lista)
{
    celda c;
    c = lista.back();
    lista.pop_back();
    return c;
}

//funcion para rellenar mapa con valores para escoger SOLO el centro de extraccion de minerales
void rellenaMapa(std::vector<std::vector<celda> >& mapa, std::list<Object*> obstacles, int nCellsWidth, int nCellsHeight,
    float mapWidth, float mapHeight)
{
    Vector3 centro(nCellsWidth/2, nCellsHeight/2);
    for(int i = 0; i < nCellsHeight; i++)
        for(int j = 0; j < nCellsWidth; j++)
        {
            mapa[i][j].row = i;
            mapa[i][j].col = j;
            mapa[i][j].valor = 0;
            for(std::list<Object*>::iterator currentObstacle = obstacles.begin(); currentObstacle != obstacles.end(); currentObstacle++)
                mapa[i][j].valor += _distance(cellToPosition(i, j, (mapWidth/nCellsWidth), (mapHeight/nCellsHeight)), (*currentObstacle)->position);
            mapa[i][j].valor *= std::min((std::min(j,nCellsWidth-j-1)), std::min(i,nCellsHeight-1-i));
        }
}

void modificaMapa(std::vector<std::vector<celda> >& mapa, Defense* centroExtraccion, 
                int nCellsWidth, int nCellsHeight)
{
    for(int i = 0; i < nCellsHeight; ++i)
        for(int j = 0; j < nCellsWidth; ++j)
            mapa[i][j].valor = _distance(Vector3(i, j), centroExtraccion->position) * std::min((std::min(j,nCellsWidth-j-1)), std::min(i,nCellsHeight-1-i));
}

void rellenaLista(std::vector<std::vector<celda> >& mapa, std::list<celda>& lista, int nCellsWidth, int nCellsHeight)
{
    for(int i = 0; i < nCellsHeight; i++)
        for(int j = 0; j < nCellsWidth; j++)
            lista.push_back(mapa[i][j]);
    lista.sort();
}

bool sortDefenses(Defense* d1, Defense* d2)
{
    return (d1->attacksPerSecond + d1->damage + d1->dispersion) > (d2->attacksPerSecond + d2->damage + d2->dispersion);
}

void DEF_LIB_EXPORTED placeDefenses(bool** freeCells, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight,
             std::list<Object*> obstacles, std::list<Defense*> defenses) 
{
    float cellWidth = mapWidth / nCellsWidth;
    float cellHeight = mapHeight / nCellsHeight; 
    std::vector<std::vector<celda> > mapa(nCellsHeight, std::vector<celda>(nCellsWidth));
    //crear una estructura para almacenar los valores de las celdas
    //para el centro de extraccion de minerales valora cada una de las celdas y guardarlas en esa estructura
    // defensa actual, guardo el puntero a la primera defensa
    /*mientras haya candidatos
        obtener la mejor posicion llamando a la funcion de seleccion
        mirar si ese candidato es factible llamando a la funcion de factibilidad
        si es factible coloco y termino, sino otro candidato */
    std::list<Defense*>::iterator centroExtraccion = defenses.begin();
    std::list<celda> listaCeldas;
    rellenaMapa(mapa, obstacles, nCellsWidth, nCellsHeight, mapWidth, mapHeight);
    rellenaLista(mapa, listaCeldas, nCellsWidth, nCellsHeight);
    bool colocado = false;
    while(!listaCeldas.empty() && !colocado) //mientras haya candidatos en la estructura (posiciones) y aun no este colocado
    {
        celda mejorPosicion = extraeCelda(listaCeldas);
        if(factible(mejorPosicion.row, mejorPosicion.col, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, defenses, *centroExtraccion))
        {
            (*centroExtraccion)->position = cellToPosition(mejorPosicion.row, mejorPosicion.col, cellWidth, cellHeight);
            colocado = true;
        }
    }
    
    //para el resto de las defensas
    //valora cada una de las celdas y guardarlas en esa estructura
    // defensa actual, guardo el puntero a la siguiente defensa
    /*mientras haya candidatos y defensa que colocar
        obtener la mejor posicion llamando a la funcion de seleccion
        mirar si ese candidato es factible llamando a la funcion de factibilidad
        si es factible coloco y termino, sino otro candidato */
    std::list<Defense*> copiaDefensas = defenses;
    copiaDefensas.pop_front();
    copiaDefensas.sort(sortDefenses);
    std::list<Defense*>::iterator currentDefense = copiaDefensas.begin();
    copiaDefensas.push_front(defenses.front()); //volvemos a introducir el centro de extraccion para que no de problemas en factible...
    std::list<celda> listaCeldas2; 
    modificaMapa(mapa, *centroExtraccion, nCellsWidth, nCellsHeight);
    rellenaLista(mapa, listaCeldas2, nCellsWidth, nCellsHeight);
    while(currentDefense != copiaDefensas.end() && !listaCeldas2.empty()) 
    {
        celda mejorPosicion = extraeCelda(listaCeldas2);
        if(factible(mejorPosicion.row, mejorPosicion.col, nCellsWidth, nCellsHeight, mapWidth, mapHeight, obstacles, copiaDefensas, *currentDefense))
        {
            (*currentDefense)->position = cellToPosition(mejorPosicion.row, mejorPosicion.col, cellWidth, cellHeight);
            ++currentDefense;
        }
    }

#ifdef PRINT_DEFENSE_STRATEGY

    float** cellValues = new float* [nCellsHeight]; 
    for(int i = 0; i < nCellsHeight; ++i) {
       cellValues[i] = new float[nCellsWidth];
       for(int j = 0; j < nCellsWidth; ++j) {
           cellValues[i][j] = ((int)(cellValue(i, j))) % 256;
       }
    }
    dPrintMap("strategy.ppm", nCellsHeight, nCellsWidth, cellHeight, cellWidth, freeCells
                         , cellValues, std::list<Defense*>(), true);

    for(int i = 0; i < nCellsHeight ; ++i)
        delete [] cellValues[i];
	delete [] cellValues;
	cellValues = NULL;

#endif
}
