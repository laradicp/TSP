#include "readData.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <float.h>

using namespace std;

double ** distancia; // matriz de adjacencia
int dimension;

typedef struct {
	double custo;
	int I;
	int K;
} tInsercao;

typedef struct {
	int I;
	int J;
	double melhorDelta;
} tVizinhanca;

void PreencheMovimentos(vector<int> &movimentosDeVizinhanca) {
	movimentosDeVizinhanca.clear();
	movimentosDeVizinhanca.push_back(0); //swap
	movimentosDeVizinhanca.push_back(1); //re-insertion
	movimentosDeVizinhanca.push_back(2); //or-opt-2
	movimentosDeVizinhanca.push_back(3); //or-opt-3
	movimentosDeVizinhanca.push_back(4); //2-opt
}

void Swap(tVizinhanca *vizinhanca, double **distancia, vector<int> &s) {
	int melhorI = 0, melhorJ = 0, tamanho = s.size();
	double deltaParcial, delta, melhorDelta = 0;
	tVizinhanca *p;
	p = vizinhanca;

	for (int i = 1; i < tamanho - 1; i++) {
		deltaParcial = -(distancia[s[i]][s[i + 1]] + distancia[s[i]][s[i - 1]]);
		for (int j = i + 1; j < tamanho - 1; j++) {
			delta = deltaParcial;
			if (j - i > 1) {
				delta += distancia[s[j]][s[i + 1]] + distancia[s[j]][s[i - 1]] + distancia[s[i]][s[j + 1]] +
					distancia[s[i]][s[j - 1]] - (distancia[s[j]][s[j + 1]] + distancia[s[j]][s[j - 1]]);
			}
			else {
				delta += distancia[s[j]][s[i - 1]] + distancia[s[i]][s[j + 1]] + distancia[s[i]][s[i + 1]] -
					distancia[s[j]][s[j + 1]];
			}

			if (delta < melhorDelta) {
				melhorDelta = delta;
				melhorI = i;
				melhorJ = j;
			}
		}
	}

	p->I = melhorI;
	p->J = melhorJ;
	p->melhorDelta = melhorDelta;
}

void N_Re_insertion (tVizinhanca *vizinhanca, double **distancia, vector<int> &s, int n) {
	int melhorI = 0, melhorJ = 0, tamanho = s.size();
	double deltaParcial, delta, melhorDelta = 0;
	tVizinhanca *p;
	p = vizinhanca;
	
	if (n == 1) {
		for (int i = 1; i < tamanho - 3; i++) {
			deltaParcial = distancia[s[i - 1]][s[i + 1]] - (distancia[s[i]][s[i - 1]] + distancia[s[i]][s[i + 1]]);
			for (int j = i + 2; j < tamanho - 1; j++) {
				delta = deltaParcial;
				delta += distancia[s[i]][s[j]] + distancia[s[i]][s[j + 1]] - distancia[s[j]][s[j + 1]];

				if (delta < melhorDelta) {
					melhorDelta = delta;
					melhorI = i;
					melhorJ = j;
				}
			}
		}

		for (int i = 3; i < tamanho - 1; i++) {
			deltaParcial = distancia[s[i - 1]][s[i + 1]] - (distancia[s[i]][s[i - 1]] + distancia[s[i]][s[i + 1]]);
			for (int j = i - 2; j > 0; j--) {
				delta = deltaParcial;
				delta += distancia[s[i]][s[j - 1]] + distancia[s[i]][s[j]] - distancia[s[j - 1]][s[j]];

				if (delta < melhorDelta) {
					melhorDelta = delta;
					melhorI = i;
					melhorJ = j;
				}
			}
		}
	}
	else {
		for (int i = 1; i < tamanho - n - 1; i++) {
			deltaParcial = distancia[s[i - 1]][s[i + n]] - (distancia[s[i]][s[i - 1]] + distancia[s[i + n - 1]][s[i + n]]);
			for (int j = i + 1; j < tamanho - n; j++) {
				delta = deltaParcial;
				delta += distancia[s[i]][s[j + n - 1]] + distancia[s[i + n - 1]][s[j + n]] - distancia[s[j + n - 1]][s[j + n]];

				if (delta < melhorDelta) {
					melhorDelta = delta;
					melhorI = i;
					melhorJ = j;
				}
			}
		}

		for (int i = n; i < tamanho - n; i++) {
			deltaParcial = distancia[s[i - 1]][s[i + n]] - (distancia[s[i]][s[i - 1]] + distancia[s[i + n - 1]][s[i + n]]);
			for (int j = i - 1; j > 0; j--) {
				delta = deltaParcial;
				delta += distancia[s[i]][s[j - 1]] + distancia[s[i + n - 1]][s[j]] - distancia[s[j - 1]][s[j]];

				if (delta < melhorDelta) {
					melhorDelta = delta;
					melhorI = i;
					melhorJ = j;
				}
			}
		}
	}

	p->I = melhorI;
	p->J = melhorJ;
	p->melhorDelta = melhorDelta;
}

void Opt_2(tVizinhanca *vizinhanca, double **distancia, vector<int> &s) {
	int melhorI = 0, melhorJ = 0, tamanho = s.size();
	double deltaParcial, delta, melhorDelta = 0;
	tVizinhanca *p;
	p = vizinhanca;

	for (int i = 1; i < tamanho - 4; i++) {
		deltaParcial = -distancia[s[i]][s[i - 1]];
		for (int j = i + 3; j < tamanho - 1; j++) {
			delta = deltaParcial;
			delta += distancia[s[j]][s[i - 1]] + distancia[s[i]][s[j + 1]] - distancia[s[j]][s[j + 1]];

			if (delta < melhorDelta) {
				melhorDelta = delta;
				melhorI = i;
				melhorJ = j;
			}
		}
	}

	p->I = melhorI;
	p->J = melhorJ;
	p->melhorDelta = melhorDelta;
}

bool Comparar(tInsercao a, tInsercao b) {
	return a.custo < b.custo;
}

int main(int argc, char** argv) {
	readData(argc, argv, &dimension, &distancia);

	srand(time(NULL));

	auto inicio = chrono::system_clock::now();

	vector<int> s, melhorS;
	double melhorCusto = DBL_MAX;
	int Iils = dimension;

	for (int cont = 0; cont < 50; cont++) {
		vector<int> listaDeCandidatos;
		for (int i = 1; i < dimension; i++) {
			listaDeCandidatos.push_back(i);
		}

		s.clear();
		int tamanhoSubtour = 3;

		s.push_back(0);
		
		for (int i = 1; i < tamanhoSubtour; i++) {
			int j = rand() % listaDeCandidatos.size();

			s.push_back(listaDeCandidatos[j]);

			listaDeCandidatos.erase(listaDeCandidatos.begin() + j);
		}

		double alfa;
		vector<tInsercao> insercao;
		tInsercao aux;

		for (; tamanhoSubtour < dimension; tamanhoSubtour++) {
			insercao.clear();

			for (int i = 1; i < tamanhoSubtour; i++) {
				int iMais1 = i + 1;
				if (iMais1 == tamanhoSubtour) iMais1 = 0;
				for (int k = 0; k < listaDeCandidatos.size(); k++) {
					aux.custo = distancia[s[i]][listaDeCandidatos[k]] + distancia[s[iMais1]][listaDeCandidatos[k]] -
						distancia[s[i]][s[iMais1]];
					aux.I = i;
					aux.K = k;
					insercao.push_back(aux);
				}
			}

			sort(insercao.begin(), insercao.end(), Comparar);

			alfa = (rand() % 100) / (float)100;

			if (insercao.size()*alfa < 1)
				alfa = (double)1 / insercao.size();

			int n = rand() % (int)round(insercao.size()*alfa);


			s.insert(s.begin() + insercao[n].I + 1, listaDeCandidatos[insercao[n].K]);
			listaDeCandidatos.erase(listaDeCandidatos.begin() + insercao[n].K);
		}

		s.push_back(s[0]);

		int tamanho = s.size();
		double custo = 0;
		for (int i = 0; i < tamanho - 1; i++) {
			custo += distancia[s[i]][s[i + 1]];
		}

		for (int iterILS = 0; iterILS < Iils; iterILS++) {
			vector<int> movimentosDeVizinhanca;
			PreencheMovimentos(movimentosDeVizinhanca);

			tVizinhanca vizinhanca;
			while (movimentosDeVizinhanca.size() > 0) {
				int vizinho = rand() % movimentosDeVizinhanca.size(), aux;

				switch (movimentosDeVizinhanca[vizinho]) {
				case 0:
					Swap(&vizinhanca, distancia, s);

					if (vizinhanca.melhorDelta < 0) {
						int aux = s[vizinhanca.I];
						s[vizinhanca.I] = s[vizinhanca.J];
						s[vizinhanca.J] = aux;

						custo += vizinhanca.melhorDelta;

						PreencheMovimentos(movimentosDeVizinhanca);
					}
					else movimentosDeVizinhanca.erase(movimentosDeVizinhanca.begin() + vizinho);

					break;

				case 1:
					N_Re_insertion (&vizinhanca, distancia, s, 1);

					if (vizinhanca.melhorDelta < 0) {
						if (vizinhanca.J > vizinhanca.I) {
							s.insert(s.begin() + vizinhanca.J + 1, s[vizinhanca.I]);
							s.erase(s.begin() + vizinhanca.I);
						}
						else {
							s.insert(s.begin() + vizinhanca.J, s[vizinhanca.I]);
							s.erase(s.begin() + vizinhanca.I + 1);
						}

						custo += vizinhanca.melhorDelta;

						PreencheMovimentos(movimentosDeVizinhanca);
					}
					else movimentosDeVizinhanca.erase(movimentosDeVizinhanca.begin() + vizinho);

					break;

				case 2:
					N_Re_insertion (&vizinhanca, distancia, s, 2);

					if (vizinhanca.melhorDelta < 0) {
						if (vizinhanca.J > vizinhanca.I) {
							s.insert(s.begin() + vizinhanca.J + 2, s[vizinhanca.I + 1]);
							s.insert(s.begin() + vizinhanca.J + 2, s[vizinhanca.I]);
							s.erase(s.begin() + vizinhanca.I + 1);
							s.erase(s.begin() + vizinhanca.I);
						}
						else {
							s.insert(s.begin() + vizinhanca.J, s[vizinhanca.I + 1]);
							s.insert(s.begin() + vizinhanca.J, s[vizinhanca.I + 1]);
							s.erase(s.begin() + vizinhanca.I + 3);
							s.erase(s.begin() + vizinhanca.I + 2);
						}

						custo += vizinhanca.melhorDelta;

						PreencheMovimentos(movimentosDeVizinhanca);
					}
					else movimentosDeVizinhanca.erase(movimentosDeVizinhanca.begin() + vizinho);

					break;

				case 3:
					N_Re_insertion (&vizinhanca, distancia, s, 3);

					if (vizinhanca.melhorDelta < 0) {
						if (vizinhanca.J > vizinhanca.I) {
							s.insert(s.begin() + vizinhanca.J + 3, s[vizinhanca.I + 2]);
							s.insert(s.begin() + vizinhanca.J + 3, s[vizinhanca.I + 1]);
							s.insert(s.begin() + vizinhanca.J + 3, s[vizinhanca.I]);
							s.erase(s.begin() + vizinhanca.I + 2);
							s.erase(s.begin() + vizinhanca.I + 1);
							s.erase(s.begin() + vizinhanca.I);
						}
						else {
							s.insert(s.begin() + vizinhanca.J, s[vizinhanca.I + 2]);
							s.insert(s.begin() + vizinhanca.J, s[vizinhanca.I + 2]);
							s.insert(s.begin() + vizinhanca.J, s[vizinhanca.I + 2]);
							s.erase(s.begin() + vizinhanca.I + 5);
							s.erase(s.begin() + vizinhanca.I + 4);
							s.erase(s.begin() + vizinhanca.I + 3);
						}

						custo += vizinhanca.melhorDelta;

						PreencheMovimentos(movimentosDeVizinhanca);
					}
					else movimentosDeVizinhanca.erase(movimentosDeVizinhanca.begin() + vizinho);

					break;

				case 4:
					Opt_2(&vizinhanca, distancia, s);
					if (vizinhanca.melhorDelta < 0) {
						for (int k = 0; k < (vizinhanca.J - vizinhanca.I + 1) / 2; k++) { //para nao excluir o i
							int aux = s[vizinhanca.I + k];
							s[vizinhanca.I + k] = s[vizinhanca.J - k];
							s[vizinhanca.J - k] = aux;
						}

						custo += vizinhanca.melhorDelta;

						PreencheMovimentos(movimentosDeVizinhanca);
					}
					else movimentosDeVizinhanca.erase(movimentosDeVizinhanca.begin() + vizinho);
				}
			}

			if (custo < melhorCusto) {
				melhorS = s;
				melhorCusto = custo;
				iterILS = -1;
			} else {
				s = melhorS; 
				custo = melhorCusto;
			}

			//perturbacao
			int n_min = 2, n_max = std::ceil(s.size() / 10.0);
			n_max = n_max >= n_min ? n_max : n_min;

			int t1 = n_min == n_max ? n_min : rand() % (n_max - n_min) + n_min;
			int i = rand() % (s.size() - 1 - t1) + 1;

			int j, t2;

			if (i < 1 + n_min)
			{
				j = rand() % ((s.size() - n_min - 1) - (i + t1) + 1) + (i + t1);
				t2 = rand() % (std::min((int)(s.size() - j - 1), n_max) - n_min + 1) + n_min;
			}
			else if (i + t1 >= s.size() - n_min)
			{
				j = rand() % (i - n_min) + 1;
				t2 = rand() % (std::min((i - j), n_max) - n_min + 1) + n_min;
			}
			else
			{
				if (rand() % 2 == 1)
				{
					j = rand() % ((s.size() - n_min - 1) - (i + t1) + 1) + (i + t1);
					t2 = rand() % (std::min((int)(s.size() - j - 1), n_max) - n_min + 1) + n_min;
				}
				else
				{
					j = rand() % (i - n_min) + 1;
					t2 = rand() % (std::min((i - j), n_max) - n_min + 1) + n_min;
				}
			}

			std::vector<int> subsequencia_i(s.begin() + i, s.begin() + i + t1);
			std::vector<int> subsequencia_j(s.begin() + j, s.begin() + j + t2);

			if (i < j)
			{
				if (j - (i + t1) > 0) {
					custo += distancia[s[j]][s[i - 1]] + distancia[s[j + t2 - 1]][s[i + t1]] +
							distancia[s[i]][s[j - 1]] + distancia[s[i + t1 - 1]][s[j + t2]] -
							(distancia[s[i]][s[i - 1]] + distancia[s[i + t1 - 1]][s[i + t1]] +
							distancia[s[j]][s[j - 1]] + distancia[s[j + t2 - 1]][s[j + t2]]);
				} else {
					custo += distancia[s[i + t1 - 1]][s[j + t2]] + distancia[s[i - 1]][s[j]] + distancia[s[j + t2 - 1]][s[i]] - 
							(distancia[s[i]][s[i - 1]] + distancia[s[j + t2 - 1]][s[j + t2]] + distancia[s[i + t1 - 1]][s[j]]);
				}

				s.erase(s.begin() + j, s.begin() + j + t2);
				s.insert(s.begin() + j, subsequencia_i.begin(), subsequencia_i.end());
				s.erase(s.begin() + i, s.begin() + i + t1);
				s.insert(s.begin() + i, subsequencia_j.begin(), subsequencia_j.end());
			}
			else
			{
				if (i - (j + t2) > 0) {
					custo += distancia[s[j]][s[i - 1]] + distancia[s[j + t2 - 1]][s[i + t1]] +
							distancia[s[i]][s[j - 1]] + distancia[s[i + t1 - 1]][s[j + t2]] -
							(distancia[s[i]][s[i - 1]] + distancia[s[i + t1 - 1]][s[i + t1]] +
							distancia[s[j]][s[j - 1]] + distancia[s[j + t2 - 1]][s[j + t2]]);
				} else {
					custo += distancia[s[j + t2 - 1]][s[i + t1]] + distancia[s[j - 1]][s[i]] + distancia[s[i + t1 - 1]][s[j]] - 
							(distancia[s[j]][s[j - 1]] + distancia[s[i + t1 - 1]][s[i + t1]] + distancia[s[j + t2 - 1]][s[i]]);
				}

				s.erase(s.begin() + i, s.begin() + i + t1);
				s.insert(s.begin() + i, subsequencia_j.begin(), subsequencia_j.end());
				s.erase(s.begin() + j, s.begin() + j + t2);
				s.insert(s.begin() + j, subsequencia_i.begin(), subsequencia_i.end());
			}
		}
	}

	auto fim = chrono::system_clock::now();
    chrono::duration<double> tempo = fim - inicio;

	for (int i = 0; i < melhorS.size(); i++) {
		printf("%d  ", melhorS[i] + 1);
	}
	printf("\nCusto: %lf\n", melhorCusto);
	printf("Tempo: %lf\n", tempo);

	return 0;
}