#include "readData.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <time.h>
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
	int melhorI = 0, melhorJ = 0;
	double delta, melhorDelta = 0;
	tVizinhanca *p;
	p = vizinhanca;

	for (int i = 1; i < s.size() - 1; i++) {
		for (int j = i + 1; j < s.size() - 1; j++) {
			if (j - i > 1) {
				delta = distancia[s[j]][s[i + 1]] + distancia[s[j]][s[i - 1]] + distancia[s[i]][s[j + 1]] +
					distancia[s[i]][s[j - 1]] - (distancia[s[i]][s[i + 1]] + distancia[s[i]][s[i - 1]] +
						distancia[s[j]][s[j + 1]] + distancia[s[j]][s[j - 1]]);
			}
			else {
				delta = distancia[s[j]][s[i - 1]] + distancia[s[i]][s[j + 1]] - (distancia[s[i]][s[i - 1]] +
					distancia[s[j]][s[j + 1]]);
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

void Re_insertion(tVizinhanca *vizinhanca, double **distancia, vector<int> &s) {
	int melhorI = 0, melhorJ = 0;
	double delta, melhorDelta = 0;
	tVizinhanca *p;
	p = vizinhanca;

	for (int i = 1; i < s.size() - 3; i++) {
		for (int j = i + 2; j < s.size() - 1; j++) {
			delta = distancia[s[i]][s[j]] + distancia[s[i]][s[j + 1]] + distancia[s[i - 1]][s[i + 1]] -
				(distancia[s[i]][s[i - 1]] + distancia[s[i]][s[i + 1]] + distancia[s[j]][s[j + 1]]);

			if (delta < melhorDelta) {
				melhorDelta = delta;
				melhorI = i;
				melhorJ = j;
			}
		}
	}

	for (int i = 3; i < s.size() - 1; i++) {
		for (int j = i - 2; j > 0; j--) {
			delta = distancia[s[i]][s[j - 1]] + distancia[s[i]][s[j]] + distancia[s[i - 1]][s[i + 1]] -
				(distancia[s[i]][s[i - 1]] + distancia[s[i]][s[i + 1]] + distancia[s[j - 1]][s[j]]);

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

void Or_opt_2(tVizinhanca *vizinhanca, double **distancia, vector<int> &s) {
	int melhorI = 0, melhorJ = 0;
	double delta, melhorDelta = 0;
	tVizinhanca *p;
	p = vizinhanca;

	for (int i = 1; i < s.size() - 3; i++) {
		for (int j = i + 1; j < s.size() - 2; j++) {
			delta = distancia[s[i]][s[j + 1]] + distancia[s[i + 1]][s[j + 2]] + distancia[s[i - 1]][s[i + 2]] -
				(distancia[s[i]][s[i - 1]] + distancia[s[i + 1]][s[i + 2]] + distancia[s[j + 1]][s[j + 2]]);

			if (delta < melhorDelta) {
				melhorDelta = delta;
				melhorI = i;
				melhorJ = j;
			}
		}
	}

	for (int i = 2; i < s.size() - 2; i++) {
		for (int j = i - 1; j > 0; j--) {
			delta = distancia[s[i]][s[j - 1]] + distancia[s[i + 1]][s[j]] + distancia[s[i - 1]][s[i + 2]] -
				(distancia[s[i]][s[i - 1]] + distancia[s[i + 1]][s[i + 2]] + distancia[s[j - 1]][s[j]]);

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

void Or_opt_3(tVizinhanca *vizinhanca, double **distancia, vector<int> &s) {
	int melhorI = 0, melhorJ = 0;
	double delta, melhorDelta = 0;
	tVizinhanca *p;
	p = vizinhanca;

	for (int i = 1; i < s.size() - 4; i++) {
		for (int j = i + 1; j < s.size() - 3; j++) {
			delta = distancia[s[i]][s[j + 2]] + distancia[s[i + 2]][s[j + 3]] + distancia[s[i - 1]][s[i + 3]] -
				(distancia[s[i]][s[i - 1]] + distancia[s[i + 2]][s[i + 3]] + distancia[s[j + 2]][s[j + 3]]);

			if (delta < melhorDelta) {
				melhorDelta = delta;
				melhorI = i;
				melhorJ = j;
			}
		}
	}

	for (int i = 3; i < s.size() - 3; i++) {
		for (int j = i - 1; j > 0; j--) {
			delta = distancia[s[i]][s[j - 1]] + distancia[s[i + 2]][s[j]] + distancia[s[i - 1]][s[i + 3]] -
				(distancia[s[i]][s[i - 1]] + distancia[s[i + 2]][s[i + 3]] + distancia[s[j - 1]][s[j]]);

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

void Opt_2(tVizinhanca *vizinhanca, double **distancia, vector<int> &s) {
	int melhorI = 0, melhorJ = 0;
	double delta, melhorDelta = 0;
	tVizinhanca *p;
	p = vizinhanca;

	for (int i = 1; i < s.size() - 4; i++) {
		for (int j = i + 3; j < s.size() - 1; j++) {
			delta = distancia[s[j]][s[i - 1]] + distancia[s[i]][s[j + 1]] - (distancia[s[i]][s[i - 1]] +
				distancia[s[j]][s[j + 1]]);

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

bool comparar(tInsercao a, tInsercao b) {
	return a.custo < b.custo;
}

int main(int argc, char** argv) {
	readData(argc, argv, &dimension, &distancia);

	srand(time(NULL));
	vector<int> s, melhorS;
	double melhorCusto = DBL_MAX;
	FILE *fp;
	int Iils;

	if (dimension >= 150) Iils = dimension / 2;
	else Iils = dimension;

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

			sort(insercao.begin(), insercao.end(), comparar);

			alfa = (rand() % 100) / (float)100;

			if (insercao.size()*alfa < 1)
				alfa = (double)1 / insercao.size();

			int n = rand() % (int)round(insercao.size()*alfa);


			s.insert(s.begin() + insercao[n].I + 1, listaDeCandidatos[insercao[n].K]);
			listaDeCandidatos.erase(listaDeCandidatos.begin() + insercao[n].K);
		}

		s.push_back(s[0]);

		double custo = 0;
		for (int i = 0; i < s.size() - 1; i++) {
			custo += distancia[s[i]][s[i + 1]];
		}

		for (int iterILS = 0; iterILS < Iils; iterILS++) {
			//tamanhoSubtour++;
			
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
					Re_insertion(&vizinhanca, distancia, s);

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
					Or_opt_2(&vizinhanca, distancia, s);

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
					Or_opt_3(&vizinhanca, distancia, s);

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

					break;
				}
			}

			if (custo < melhorCusto) {
				melhorS = s;
				melhorCusto = custo;
				iterILS = -1;
			}
		
			//perturbacao
			int ar[4];

			for (int i = 0; i < 4; i++){
				ar[i] = rand() % (s.size() - 5) + 1;
			}
			sort(ar, ar + 4);

			for (int i = 0; i < 4; i++){
				if (ar[i + 1] <= ar[i])
					ar[i + 1] = ar[i] + 1;
			}
			
			if (ar[1] - ar[0] > dimension / 10 - 1)
				ar[1] = ar[0] + dimension / 10 - 1;
		
			if (ar[3] - ar[2] > dimension / 10 - 1)
				ar[3] = ar[2] + dimension / 10 - 1;
			
			int tamanho1 = ar[1] - ar[0] + 1, tamanho2 = ar[3] - ar[2] + 1;

			if (ar[2] - ar[1] > 1)
				custo += distancia[s[ar[3]]][s[ar[0] - 1]] + distancia[s[ar[2]]][s[ar[1] + 1]] +
						 distancia[s[ar[1]]][s[ar[2] - 1]] + distancia[s[ar[0]]][s[ar[3] + 1]] -
						 (distancia[s[ar[0]]][s[ar[0] - 1]] + distancia[s[ar[1]]][s[ar[1] + 1]] +
						 distancia[s[ar[2]]][s[ar[2] - 1]] + distancia[s[ar[3]]][s[ar[3] + 1]]);
			else
				custo += distancia[s[ar[0]]][s[ar[3] + 1]] + distancia[s[ar[0] - 1]][s[ar[3]]] - 
						 (distancia[s[ar[0]]][s[ar[0] - 1]] + distancia[s[ar[3]]][s[ar[3] + 1]]);

			for (int i = tamanho1 - 1; i >= 0; i--) {
				s.insert(s.begin() + ar[2], s[ar[0] + i]);
				s.erase(s.begin() + ar[0] + i);
			}

			for (int i = 0; i < tamanho2; i++) {
				s.insert(s.begin() + ar[0], s[ar[2] + i]);
				s.erase(s.begin() + ar[2] + i + 1);
			}
		}
	}
	for (int i = 0; i < melhorS.size(); i++) {
		printf("%d  ", melhorS[i] + 1);
	}
	printf("\nCusto: %lf\n", melhorCusto);

  return 0;
}
