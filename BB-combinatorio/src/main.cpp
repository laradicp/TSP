#include <iostream>
#include <vector>
#include "readData.h"
#include "hungarian.h"

typedef struct
{
    std::vector<std::pair<int, int>> arcosProibidos; //lista de arcos proibidos
    std::vector<std::vector<int>> subtours; //conjunto de subtours da solução gerada pelo algoritmo húngaro
    double lowerBound; //lower bound produzida pelo nó (ou custo total da solução do algoritmo húngaro)
    int escolhido; //subtour escolhido dado o critério de seleção
    bool podar; //variável que diz se o nó deve gerar filhos
} Node;

double **distancia; // matriz de adjacencia
int dimension;

void calcularSolucao(hungarian_problem_t *p, Node &node)
{
    node.lowerBound = hungarian_solve(p);
    std::cout << "Custo:\t" << node.lowerBound << std::endl;

    std::cout << "Arcos proibidos:" << std::endl;
    for(int i = 0; i < node.arcosProibidos.size(); i++)
    {
        std::cout << "\t(" << node.arcosProibidos[i].first << ", " << node.arcosProibidos[i].second << ")" << std::endl;
    }

    std::vector<int> listaDeCandidatos;

    for(int i = 0; i < dimension; i++)
    {
        listaDeCandidatos.push_back(i);
    }

    std::cout << "Subtours:" << std::endl;
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

        std::cout << "\t";
        for(int j = 0; j < subtour.size(); j++)
        {
            std::cout << subtour[j] << "  ";
        }
        std::cout << std::endl;

        node.subtours.push_back(subtour);
    }
    std::cout << std::endl;

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

int main(int argc, char **argv)
{
    double custoMin;

    if (argc == 3)
        custoMin = atof(argv[2]);
    else
        custoMin = 99999999;
    
	readData(argc, argv, &dimension, &distancia);
    
    double **distanciaModificada = new double *[dimension];
    for(int i = 0; i < dimension; i++)
    {
        distanciaModificada[i] = new double [dimension];
        distancia[i][i] = 99999999;
    }

    //criação da árvore
    std::vector<Node> arvore;
    Node raiz, bestNode;
    int iter = 0;

    bestNode.lowerBound = custoMin;

    hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
    
    hungarian_init(&p, distancia, dimension, dimension, mode);

    for(int i = 0; i < dimension; i++)
    {
        for(int j = 0; j < dimension; j++)
        {
            std::cout << distancia[i][j] << "  ";
        }
        std::cout << std::endl;
    }
    
    calcularSolucao(&p, raiz); //calcular solução usando algoritmo húngaro e matriz de distância inicial e preencher os atributos da estrutura Node de acordo
    
    hungarian_free(&p);

    //adicionar os primeiros nós
    int tamanho = raiz.subtours[raiz.escolhido].size();
    if(tamanho == 3)
        tamanho--;
        
    for(int i = 0; i < tamanho - 1; i++)
    {
        Node newNode;
        newNode.arcosProibidos = raiz.arcosProibidos;

        std::pair<int, int> arcoProibido;
        arcoProibido.first = raiz.subtours[raiz.escolhido][i];
        arcoProibido.second = raiz.subtours[raiz.escolhido][i + 1];

        newNode.arcosProibidos.push_back(arcoProibido);

        newNode.lowerBound = raiz.lowerBound; 
        
        arvore.push_back(newNode);
    }

    while(!arvore.empty())
    {
        //std::cout << arvore.size() << std::endl;
        
        for(int i = 0; i < dimension; i++)
        {
            for(int j = 0; j < dimension; j++)
            {
                distanciaModificada[i][j] = distancia[i][j];
            }
        }
        
        tamanho = arvore[iter].arcosProibidos.size();
        for(int i = 0; i < tamanho; i++)
        {
            distanciaModificada[arvore[iter].arcosProibidos[i].first][arvore[iter].arcosProibidos[i].second] = 99999999;
            distanciaModificada[arvore[iter].arcosProibidos[i].second][arvore[iter].arcosProibidos[i].first] = 99999999;
        }
        
        hungarian_init(&p, distanciaModificada, dimension, dimension, mode);

        for(int i = 0; i < dimension; i++)
        {
            for(int j = 0; j < dimension; j++)
            {
                std::cout << distanciaModificada[i][j] << "  ";
            }
            std::cout << std::endl;
        } 

        calcularSolucao(&p, arvore[iter]);

        hungarian_free(&p);

        if(!arvore[iter].podar)
        {
            tamanho = arvore[iter].subtours[arvore[iter].escolhido].size();
            if(tamanho == 3)
                tamanho--;
                
            for(int i = 0; i < tamanho - 1; i++)
            {
                Node newNode;
                newNode.arcosProibidos = arvore[iter].arcosProibidos;

                std::pair<int, int> arcoProibido;
                arcoProibido.first = arvore[iter].subtours[arvore[iter].escolhido][i];
                arcoProibido.second = arvore[iter].subtours[arvore[iter].escolhido][i + 1];

                newNode.arcosProibidos.push_back(arcoProibido);

                newNode.lowerBound = arvore[iter].lowerBound; 
                
                arvore.push_back(newNode);
            }   
                
            arvore.erase(arvore.begin() + iter);
        }
        else if(arvore[iter].lowerBound < custoMin)
        {
            custoMin = arvore[iter].lowerBound;
            
            bestNode = arvore[iter];
            arvore.erase(arvore.begin() + iter);

            std::cout << bestNode.lowerBound << std::endl;
        }

        iter = 0;
        tamanho = arvore.size();
        for(int i = 0; i < tamanho; i++)
        {
            if(arvore[i].lowerBound > custoMin)
            {
                arvore.erase(arvore.begin() + i);
                tamanho--;
                i--;
                continue;
            }

            if(arvore[i].lowerBound < arvore[iter].lowerBound)
                iter = i;
        }
    }

    if(bestNode.subtours.size())
    {
        double custo = 0;
        for(int i = 0; i < bestNode.subtours[0].size() - 1; i++)
        {
            custo += distancia[bestNode.subtours[0][i]][bestNode.subtours[0][i + 1]];
            std::cout << bestNode.subtours[0][i] << "  ";
        }
        std::cout << std::endl << "Lower bound: " << bestNode.lowerBound << std::endl << "Custo: " << custo << std::endl;
    }
    else
        std::cout << "Limite superior inviavel para instancia" << std::endl;

    for (int i = 0; i < dimension; i++)
    {
        delete[] distanciaModificada[i];
    }
	delete[] distanciaModificada;

    return 0;
}