// ###### Config options ################

#define PRINT_PATHS 1

// #######################################

#define BUILDING_DEF_STRATEGY_LIB 1

#include "../simulador/Asedio.h"
#include "../simulador/Defense.h"

#ifdef PRINT_PATHS
#include "ppm.h"
#endif

using namespace Asedio;

Vector3 cellCenterToPosition(int i, int j, float cellWidth, float cellHeight)
{ 
    return Vector3((j * cellWidth) + cellWidth * 0.5f, (i * cellHeight) + cellHeight * 0.5f, 0); 
}

void positionToCell(const Vector3 pos, int &i_out, int &j_out, float cellWidth, float cellHeight)
{ 
    i_out = (int)(pos.y * 1.0f/cellHeight); 
    j_out = (int)(pos.x * 1.0f/cellWidth); 
}

void DEF_LIB_EXPORTED calculateAdditionalCost(float** additionalCost, int cellsWidth, int cellsHeight, float mapWidth, float mapHeight
            , List<Object*> obstacles, List<Defense*> defenses) 
{	
    float cellWidth = mapWidth / cellsWidth;
    float cellHeight = mapHeight / cellsHeight;
    
    for(int i = 0 ; i < cellsHeight ; ++i) 
    {
        for(int j = 0 ; j < cellsWidth ; ++j) 
        {
            Vector3 cellPosition = cellCenterToPosition(i, j, cellWidth, cellHeight);
            float cost = 0;
            if( (i+j) % 2 == 0 ) {
                cost = cellWidth * 100;
            }
            if(additionalCost[i][j] != 0)
            	additionalCost[i][j] += cost;
            else 
            	additionalCost[i][j] -= cost;
        }
    }
    for(std::list<Defense*>::iterator defense = defenses.begin(); defense != defenses.end(); defense++) 
    {
        int row,col;
		positionToCell((*defense)->position, row, col, cellsWidth, cellsHeight);
        additionalCost[row][col] = 0;
    }
}

double estimatedDistance(AStarNode* cur, AStarNode* target, int row, int col, float** additionalCost)
{
    return _distance(cur->position, target->position) + additionalCost[row][col];
}

bool iguales(const AStarNode* nodo1, const AStarNode* nodo2) //realmente estoy comparando punteros.... ya se queda asi
{
	return nodo1->F == nodo2->F && nodo1->G == nodo2->G && nodo1->parent == nodo2->parent && nodo1->position.x == nodo2->position.x && nodo1->position.y == nodo2->position.y;
}

struct isEqualTo 
{
    isEqualTo(AStarNode* i) : i_{i} {}
    bool operator()(AStarNode* i) { return iguales(i, i_); }
    AStarNode* i_;
};


bool min(const AStarNode* nodo1, const AStarNode* nodo2)
{
    return nodo2->F < nodo1->F;
}

void imprimir(const std::vector<AStarNode*>& v)
{
	for(int i = 0; i < v.size(); i++)
	{
		std::cout << v[i]->F << " ";
	}
	std::cout << std::endl;
}

void DEF_LIB_EXPORTED calculatePath(AStarNode* originNode, AStarNode* targetNode
                   , int cellsWidth, int cellsHeight, float mapWidth, float mapHeight
                   , float** additionalCost, std::list<Vector3> &path) 
{
    bool found = false;
    std::vector<AStarNode*> closed, opened;
    AStarNode* cur = originNode;
    int x, y;
    positionToCell((cur)->position, x, y, cellsWidth, cellsHeight);
    cur->H = estimatedDistance(originNode, targetNode, x, y, additionalCost);
    cur->G = 0; 
    cur->F = cur->G + cur->H;
    opened.push_back(cur);
    std::make_heap(opened.begin(), opened.end(), min);
    while(!found && opened.size() > 0)
    {
    	//std::cout << "Antes de hacer pop " << std::endl;
    	//imprimir(opened);
        std::pop_heap(opened.begin(), opened.end(), min);
        cur = opened.back();
        opened.pop_back(); 
        //std::cout << "Escogido: "<< cur->F << std::endl;
        closed.push_back(cur);
		//std::cout << "Despues de hacer pop " << std::endl;
		//imprimir(opened);
        if(iguales(cur, targetNode)) //podria hacer simplemente cur == targetNode
            found = true;
        else
        {
            List<AStarNode*>::iterator j; 
            for(j = cur->adjacents.begin(); j != cur->adjacents.end(); ++j) 
            {
                if(std::find_if(closed.begin(), closed.end(), isEqualTo(*j)) == std::end(closed)) //no se encuentra en cerrados
                {
                    if(std::find_if(opened.begin(), opened.end(), isEqualTo(*j)) == std::end(opened)) //no se encuentra en abiertos
                    {
                        (*j)->parent = cur;
                        (*j)->G = cur->G + _distance(cur->position, (*j)->position);
                        int row,col;
                        positionToCell((*j)->position, row, col, cellsWidth, cellsHeight);
                        (*j)->H = estimatedDistance(*j, targetNode, row, col, additionalCost);
                        (*j)->F = (*j)->G + (*j)->H;
                        opened.push_back(*j);
                        std::push_heap(opened.begin(), opened.end(), min);
                    }
                    else //se ha encontrado en abiertos
                    {
                        float d = _distance(cur->position, (*j)->position);
                        if((*j)->G > cur->G + d)
                        {
                            (*j)->parent = cur;
                            (*j)->G = cur->G + d;
                            (*j)->F = (*j)->G + (*j)->H;
                            std::sort_heap(opened.begin(), opened.end(), min);
                        }
                    }
                }
            }
        }
    }
    if(found)
    {
        cur = targetNode;
        path.push_front(targetNode->position);
		while(cur->parent != originNode)
		{
			cur = cur->parent;
			path.push_front(cur->position);
		}
    }
}
