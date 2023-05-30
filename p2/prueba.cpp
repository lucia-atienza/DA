#include <iostream>
#include <vector>


void rellenarTSR(std::vector<std::vector<int> >& TSP, const std::vector<int>& valores, const std::vector<int> defensas)
{
    for(int i = 0; i < TSP[0].size(); ++i)
    {
        if(i < defensas[0])
            TSP[0][i] = 0;
        else 
            TSP[0][i] = valores[0];
    }
    
    for(int i = 1; i < defensas.size(); ++i)
    {
        for(int j = 0; j < TSP[0].size(); ++j)
        {
            if(j < defensas[i])
                TSP[i][j] = TSP[i-1][j];
            else
                TSP[i][j] = std::max(TSP[i-1][j], (TSP[i-1][j-defensas[i]] + valores[i]));
        }
    }
}

int main()
{
    std::vector<int> valores{1, 6, 18, 22, 28};
    std::vector<int> pesos{1, 2, 5, 6, 7};
    std::vector<std::vector<int> > TSP(5, std::vector<int>(10+1)); 
    rellenarTSR(TSP, valores, pesos);
    for(int i = 0; i < TSP.size(); ++i)
    {       
        for(int j = 0; j < TSP[0].size(); ++j)
        {
            std::cout << TSP[i][j] << " ";
        }
        std::cout << std::endl;
    }

}
