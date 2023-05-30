#include <iostream>
#include <vector>
#include <algorithm>
#include <initializer_list>

using std::cout; using std::endl;
using std::string; using std::vector;
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

void insertionSort(std::vector<int>& v, int i, int j) 
{
    for(int pos = i; pos < j; ++pos)
    {
        int aux = pos;
        int x = v[aux];
        while (aux != i && x < v[aux - 1]) 
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

template<typename T>
void printVector(const std::vector<T> &vec) {
    for (int i = 0; i < vec.size(); i++) {
        std::cout << vec[i] << "; ";
    }
    std::cout << endl;
}



int main()
{
    std::vector<int> vec1;
    for(int i = 0; i < 5; i++)
        vec1.push_back(10-i);
    printVector(vec1);
    ordenacion_rapida(vec1, 0, vec1.size());
    printVector(vec1);

}