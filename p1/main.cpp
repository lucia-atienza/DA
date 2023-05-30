#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
struct celda
{
    double valor;
    int row, col;
    celda() : valor(0), row(0), col(0) {}
    celda(double valor, int row, int col) : valor(valor), row(row), col(col) {}
    bool operator<(const celda& c1) const { return valor < c1.valor; }
};
bool comparaCeldas(const celda& c1, const celda& c2) 
{
    return c1.valor < c2.valor;
}
void rellenaCelda(std::vector<std::vector<celda> >& mapa, int row, int col, int nCellsWidth, int nCellsHeight)
{
    if(row >= 0 && row < nCellsHeight && col >= 0 && col < nCellsWidth)
        mapa[row][col].valor++;
}

void rellenaMapa(std::vector<std::vector<celda> >& mapa, int nCellsWidth, int nCellsHeight, float mapWidth, float mapHeight)
{
    for(int i = 0; i < nCellsWidth; i++)
        for(int j = 0; j < nCellsWidth; j++)
        {
            mapa[i][j].row = i;
            mapa[i][j].col = j;
            mapa[i][j].valor = std::min((std::min(j,nCellsWidth-j-1)), std::min(i,nCellsHeight-1-i));
        }

}
void rellenaLista(std::vector<std::vector<celda> >& mapa, std::list<celda>& lista, int nCellsWidth, int nCellsHeight)
{
    for(int i = 0; i < nCellsHeight; i++)
        for(int j = 0; j < nCellsWidth; j++)
            lista.push_back(mapa[i][j]);
    lista.sort(comparaCeldas);
}
int main()
{
    std::vector<std::vector<celda> > mapa(5, std::vector<celda>(5));
    std::list<celda> lista;
    rellenaMapa(mapa, 5, 5, 5, 5);
    rellenaLista(mapa, lista, 5, 5);
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 5; j++)
        {
            std::cout << mapa[i][j].valor << " ";
        }
        std::cout << std::endl;
    }
    
}