#include <iostream>
#include <vector>
#include<stdlib.h>
#include<time.h>
#include <algorithm>


void insertionSort(std::vector<int>& v, int i, int j) 
{
    for(int pos = i; pos < j; ++pos)
    {
        int aux = pos;
        int x = v[aux];
        while(aux != i && x < v[aux - 1]) 
        {
            v[aux] = v[aux - 1];
            --aux;
        }
        v[aux] = x;
    }
}

int pivote(std::vector<int>& v, int i, int j)
{
    int p = i;
    int x = v[i];
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
void fusion(std::vector<int>& v, int i, int k, int j)
{
    int n = j - i;
    int p = i, q = k;
    std::vector<int> w(v.size());
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

void ordenacion_fusion(std::vector<int>& v, int i, int j)
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

void ordenacion_rapida(std::vector<int>& v, int i, int j)
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



bool compruebaOrdenado(std::vector<int> v)
{
    for(int i = 0; i < v.size()-1; i++)
    {
        if(v[i] > v[i+1])
            return false;
    }
    return true;
}

int main()
{
    std::vector<int> v;
    std::cout << "[Ordenacion rapida] Para un vector de 5 elementos, 120 combinaciones posibles" << std::endl; 
    for(int i = 0; i < 5; i++)
        v.push_back(i);
    int ordenadas = 0;
    do{
        std::vector<int> aux = v;
        ordenacion_rapida(aux, 0, aux.size());
        if(compruebaOrdenado(aux))
            ordenadas++;
    }while(std::next_permutation(v.begin(), v.end()));
    std::cout << "Numero de permutaciones ordenadas: " << ordenadas << std::endl; 
}