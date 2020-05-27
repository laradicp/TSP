#include <iostream>
#include <vector>
#include <chrono>
#include "readData.h"
#include "hungarian.h"

typedef struct
{
    std::vector<std::pair<int, int>> arcosProibidos; //lista de arcos proibidos
    std::vector<std::vector<int>> subtours; //conjunto de subtours da solução gerada pelo algoritmo húngaro
    double lowerBound; //lower bound produzida pelo nó (ou custo total da solução do algoritmo húngaro)
    int escolhido; //subtour escolhido dado o critério de seleção
    bool podar; //variável que diz se o nó deve gerar filhos
    std::chrono::duration<double> tempo; //tempo de duração da busca pelo ótimo
} Node;

double **distancia; //matriz de adjacência
int dimension;

void calcularSolucao(hungarian_problem_t *p, Node &node)
{
    std::vector<int> listaDeCandidatos;
    for(int i = 0; i < dimension; i++)
    {
        listaDeCandidatos.push_back(i);
    }

    while(!listaDeCandidatos.empty())
    {
        int inicio = listaDeCandidatos[0];
        int i = inicio;

        std::vector<int> subtour;
        subtour.push_back(i);

        for(int j = 0; j < listaDeCandidatos.size(); j++)
        {
            if(p->assignment[i][listaDeCandidatos[j]])
            {
                subtour.push_back(listaDeCandidatos[j]);
                i = listaDeCandidatos[j];
                listaDeCandidatos.erase(listaDeCandidatos.begin() + j);
                
                if(i == inicio)
                    break;
                
                j = -1;
            }
        }

        node.subtours.push_back(subtour);
    }

    int tamanho = node.subtours.size();

    if(tamanho == 1)
        node.podar = true;
    else
    {
        node.podar = false;

        int subtourMenor = 0;
        
        for(int i = 1; i < tamanho; i++)
        {
            if(node.subtours[i].size() < node.subtours[subtourMenor].size())
                subtourMenor = i;
        }

        node.escolhido = subtourMenor;
    }
}

Node melhorBound(std::vector<Node> arvore, double custoMin, hungarian_problem_t *p, int mode){
    auto inicio = std::chrono::system_clock::now();
    Node bestNode;
    int iter = 0;

    double **distanciaModificada = new double *[dimension];
    for(int i = 0; i < dimension; i++)
    {
        distanciaModificada[i] = new double [dimension];
        distancia[i][i] = 99999999;
    }

    while(!arvore.empty())
    {
        int tamanho1;

        if(!arvore[iter].podar)
        {
            tamanho1 = arvore[iter].subtours[arvore[iter].escolhido].size();
            for(int i = 0; i < tamanho1 - 1; i++)
            {
                Node newNode;
                newNode.arcosProibidos = arvore[iter].arcosProibidos;

                std::pair<int, int> arcoProibido;
                arcoProibido.first = arvore[iter].subtours[arvore[iter].escolhido][i];
                arcoProibido.second = arvore[iter].subtours[arvore[iter].escolhido][i + 1];
                newNode.arcosProibidos.push_back(arcoProibido);

                for(int j = 0; j < dimension; j++)
                {
                    for(int k = 0; k < dimension; k++)
                    {
                        distanciaModificada[j][k] = distancia[j][k];
                    }
                }
                
                int tamanho2 = newNode.arcosProibidos.size();
                for(int j = 0; j < tamanho2; j++)
                {
                    distanciaModificada[newNode.arcosProibidos[j].first][newNode.arcosProibidos[j].second] = 99999999;
                }
                
                hungarian_init(p, distanciaModificada, dimension, dimension, mode);

                newNode.lowerBound = hungarian_solve(p);
                if(newNode.lowerBound <= custoMin)
                {
                    calcularSolucao(p, newNode);
                    arvore.push_back(newNode);
                }

                hungarian_free(p);
            }   
                
            arvore.erase(arvore.begin() + iter);
        }
        else if(arvore[iter].lowerBound < custoMin)
        {
            custoMin = arvore[iter].lowerBound;
            
            bestNode = arvore[iter];
            arvore.erase(arvore.begin() + iter);
        }

        iter = 0;
        tamanho1 = arvore.size();
        for(int i = 0; i < tamanho1; i++)
        {
            if(arvore[i].lowerBound >= custoMin)
            {
                arvore.erase(arvore.begin() + i);
                tamanho1--;
                i--;
                continue;
            }

            if(arvore[i].lowerBound < arvore[iter].lowerBound)
                iter = i;
        }

        auto fim = std::chrono::system_clock::now();
        bestNode.tempo = fim - inicio;
    
        if(bestNode.tempo.count() > 600)
            break;
    }

    for (int i = 0; i < dimension; i++)
    {
        delete[] distanciaModificada[i];
    }
	delete[] distanciaModificada;

    return bestNode;
}

Node largura(std::vector<Node> arvore, double custoMin, hungarian_problem_t *p, int mode){
    auto inicio = std::chrono::system_clock::now();
    Node bestNode;
    int iter;

    double **distanciaModificada = new double *[dimension];
    for(int i = 0; i < dimension; i++)
    {
        distanciaModificada[i] = new double [dimension];
        distancia[i][i] = 99999999;
    }

    while(!arvore.empty())
    {
        iter = 0;
        int tamanho1;

        if(!arvore[iter].podar)
        {
            tamanho1 = arvore[iter].subtours[arvore[iter].escolhido].size();
            for(int i = 0; i < tamanho1 - 1; i++)
            {
                Node newNode;
                newNode.arcosProibidos = arvore[iter].arcosProibidos;

                std::pair<int, int> arcoProibido;
                arcoProibido.first = arvore[iter].subtours[arvore[iter].escolhido][i];
                arcoProibido.second = arvore[iter].subtours[arvore[iter].escolhido][i + 1];
                newNode.arcosProibidos.push_back(arcoProibido);

                for(int j = 0; j < dimension; j++)
                {
                    for(int k = 0; k < dimension; k++)
                    {
                        distanciaModificada[j][k] = distancia[j][k];
                    }
                }
                
                int tamanho2 = newNode.arcosProibidos.size();
                for(int j = 0; j < tamanho2; j++)
                {
                    distanciaModificada[newNode.arcosProibidos[j].first][newNode.arcosProibidos[j].second] = 99999999;
                }
                
                hungarian_init(p, distanciaModificada, dimension, dimension, mode);

                newNode.lowerBound = hungarian_solve(p);
                if(newNode.lowerBound <= custoMin)
                {
                    calcularSolucao(p, newNode);
                    arvore.push_back(newNode);
                }

                hungarian_free(p);
            }   
                
            arvore.erase(arvore.begin() + iter);
        }
        else if(arvore[iter].lowerBound < custoMin)
        {
            custoMin = arvore[iter].lowerBound;
            
            bestNode = arvore[iter];
            arvore.erase(arvore.begin() + iter);
        }

        tamanho1 = arvore.size();
        for(int i = 0; i < tamanho1; i++)
        {
            if(arvore[i].lowerBound >= custoMin)
            {
                arvore.erase(arvore.begin() + i);
                tamanho1--;
                i--;
                continue;
            }
        }

        auto fim = std::chrono::system_clock::now();
        bestNode.tempo = fim - inicio;

        if(bestNode.tempo.count() > 600)
            break;
    }

    for (int i = 0; i < dimension; i++)
    {
        delete[] distanciaModificada[i];
    }
	delete[] distanciaModificada;

    return bestNode;
}

Node profundidade(std::vector<Node> arvore, double custoMin, hungarian_problem_t *p, int mode){
    auto inicio = std::chrono::system_clock::now();
    Node bestNode;
    int iter;

    double **distanciaModificada = new double *[dimension];
    for(int i = 0; i < dimension; i++)
    {
        distanciaModificada[i] = new double [dimension];
        distancia[i][i] = 99999999;
    }

    int tamanho = arvore.size();
    while(tamanho)
    {
        iter = tamanho - 1;
        int tamanho1;

        if(!arvore[iter].podar)
        {
            tamanho1 = arvore[iter].subtours[arvore[iter].escolhido].size();
            for(int i = 0; i < tamanho1 - 1; i++)
            {
                Node newNode;
                newNode.arcosProibidos = arvore[iter].arcosProibidos;

                std::pair<int, int> arcoProibido;
                arcoProibido.first = arvore[iter].subtours[arvore[iter].escolhido][i];
                arcoProibido.second = arvore[iter].subtours[arvore[iter].escolhido][i + 1];
                newNode.arcosProibidos.push_back(arcoProibido);

                for(int j = 0; j < dimension; j++)
                {
                    for(int k = 0; k < dimension; k++)
                    {
                        distanciaModificada[j][k] = distancia[j][k];
                    }
                }
                
                int tamanho2 = newNode.arcosProibidos.size();
                for(int j = 0; j < tamanho2; j++)
                {
                    distanciaModificada[newNode.arcosProibidos[j].first][newNode.arcosProibidos[j].second] = 99999999;
                }
                
                hungarian_init(p, distanciaModificada, dimension, dimension, mode);

                newNode.lowerBound = hungarian_solve(p);
                if(newNode.lowerBound <= custoMin)
                {
                    calcularSolucao(p, newNode);
                    arvore.push_back(newNode);
                }

                hungarian_free(p);
            }   
                
            arvore.erase(arvore.begin() + iter);
        }
        else if(arvore[iter].lowerBound < custoMin)
        {
            custoMin = arvore[iter].lowerBound;
            
            bestNode = arvore[iter];
            arvore.erase(arvore.begin() + iter);
        }

        tamanho1 = arvore.size();
        for(int i = 0; i < tamanho1; i++)
        {
            if(arvore[i].lowerBound >= custoMin)
            {
                arvore.erase(arvore.begin() + i);
                tamanho1--;
                i--;
                continue;
            }
        }

        tamanho = arvore.size();

        auto fim = std::chrono::system_clock::now();
        bestNode.tempo = fim - inicio;

        if(bestNode.tempo.count() > 600)
            break;
    }

    for (int i = 0; i < dimension; i++)
    {
        delete[] distanciaModificada[i];
    }
	delete[] distanciaModificada;

    return bestNode;
}

void printSolucao(Node bestNode)
{
    if(bestNode.subtours.size())
    {
        int tamanho = bestNode.subtours[0].size();
        for(int i = 0; i < tamanho; i++)
        {
            std::cout << bestNode.subtours[0][i] << "  ";
        }
        std::cout << std::endl << "Custo: " << bestNode.lowerBound << std::endl;
    }
    else
        std::cout << "Nenhuma solucao viavel encontrada" << std::endl;

    std::cout << "Tempo: " << bestNode.tempo.count() << std::endl;
}

int main(int argc, char **argv)
{
    double custoMin;

    if (argc == 3)
        custoMin = atof(argv[2]);
    else
        custoMin = 99999999;
    
	readData(argc, argv, &dimension, &distancia);
    
    for(int i = 0; i < dimension; i++)
    {
        distancia[i][i] = 99999999;
    }

    hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
    
    hungarian_init(&p, distancia, dimension, dimension, mode);

    //criação da árvore
    std::vector<Node> arvore;
    Node raiz, bestNode;

    raiz.lowerBound = hungarian_solve(&p);
    calcularSolucao(&p, raiz); //calcular solução usando algoritmo húngaro e matriz de distância inicial e preencher os atributos da estrutura Node de acordo
    
    arvore.push_back(raiz);

    hungarian_free(&p);

    std::cout << "Melhor bound:" << std::endl;
    bestNode = melhorBound(arvore, custoMin, &p, mode);
    printSolucao(bestNode);
    std::cout << std::endl;

    std::cout << "Largura:" << std::endl;
    bestNode = largura(arvore, custoMin, &p, mode);
    printSolucao(bestNode);
    std::cout << std::endl;

    std::cout << "Profundidade:" << std::endl;
    bestNode = profundidade(arvore, custoMin, &p, mode);
    printSolucao(bestNode);
    std::cout << std::endl;

    return 0;
}