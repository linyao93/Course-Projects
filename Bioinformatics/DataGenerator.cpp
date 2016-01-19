#include <iostream>
#include <stdlib.h>
#include <time.h>


using namespace std;

#define SEEDNUM 8
#define GENEPERSEED 8
#define GENENUM 20 //(SEEDNUM*GENEPERSEED)
#define SNPPERGENE 10
#define SNPNUM 400//(GENENUM*SNPPERGENE*2)
#define VECLEN 50
#define RANGE 100
#define ERR 5

int main(){
//	int seed[SEEDNUM][VECLEN] = { 0 };
	int gene[VECLEN] = { 0 }, snp[2][VECLEN] = { 0 }, snp_sum[VECLEN] = { 0 };
//	int snpgen[SNPNUM] = { 0 };
	FILE *gf,*sf,*rf;

	srand((unsigned)time(NULL));
	/*
	for (int i = 0; i < SEEDNUM; i++){
		for (int j = 0; j < VECLEN; j++){
			seed[i][j] = rand() % RANGE;
			for (int k = 0; k < GENEPERSEED; k++){
				int genid = i*GENEPERSEED + k;
				geneseed[genid] = i;
				gene[genid][j] = seed[i][j] += rand() % ERR;
				for (int p = 0; p < SNPPERGENE; p++){
					int snpid = genid*SNPPERGENE + p;
					snpgen[snpid] = genid;
					snpgen[SNPNUM - snpid - 1] = genid;
					snp[snpid][j] = rand() % gene[genid][j];
					snp[SNPNUM - snpid - 1][j] = gene[genid][j] - snp[snpid][j];
				}
			}
		}
	}
*/
	fopen_s(&gf, "gene.txt", "w");
	fopen_s(&sf, "snp.txt", "w");
	fopen_s(&rf, "snpgen.txt", "w");

	for (int k = 0; k < GENENUM; k++){
		printf("gene No.%d\n", k);
		for (int j = 0; j < VECLEN; j++){
			gene[j] = rand() % RANGE;
			if (gene[j]>100)
				gene[j] -= 2 * ERR;
			fprintf(gf, "%d\t", gene[j]);
		}
		fprintf(gf, "\n");
		for (int p = 0; p < SNPPERGENE; p++){
			int snpid = k*SNPPERGENE + p;
			for (int j = 0; j < VECLEN; j++){
				snp_sum[j] = gene[j] + ((rand() % 2) ? rand() % ERR : -rand() % ERR);
				if (snp_sum[j] <= 0)
					snp_sum[j] += 2 * ERR;
				if (snp_sum[j] > 100)
					snp_sum[j] -= 2 * ERR;
				snp[0][j] = rand() % snp_sum[j];
				fprintf(sf, "%d\t", snp[0][j]);
				snp[1][j] = snp_sum[j] - snp[0][j];
			}
			fprintf(sf, "\n");
			for (int j = 0; j < VECLEN; j++){
				fprintf(sf, "%d\t", snp[1][j]);
			}
			fprintf(sf, "\n");
		}
	}

	fclose(gf);
	fclose(sf);
	/*
	fopen_s(&sf, "snpsum.txt", "w");
	for (int i = 0; i < SNPNUM/2; i++){
		for (int j = 0; j < VECLEN; j++){
			fprintf(pf, "%d\t", snp_sum[i][j]);
		}
		fprintf(pf, "\n");
	}
	*/
	
	/*
	fopen_s(&pf, "geneseed.txt", "w");
	for (int i = 0; i < GENENUM; i++){
		fprintf(pf, "%d\n", geneseed[i]);
	}
	fclose(pf);
	
	
	for (int i = 0; i < SNPNUM; i++){
		fprintf(rf, "%d\t%d\n", snpgen[i], SNPNUM - i - 1);
	}
	fclose(rf);
*/
	getchar();
	return 0;
}
