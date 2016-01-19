#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define GENENUM 1000 //(SEEDNUM*GENEPERSEED)
#define SNPPERGENE 10
#define SNPNUM 10000//(GENENUM*SNPPERGENE*2)
#define VECLEN 541

#define TH 0.98
#define SQR2 1.414
#define T2 0.4057

using namespace std;

struct Gene{
//	char name[255];
	float gene[VECLEN];
};
struct Snp{
//	char name[255];
	float snp[VECLEN];
	int gene = 0;
	int peer = 0;
};
Gene gene, gen[GENENUM];
Snp snps, snpsum, snp[SNPNUM];
int candnum = 0, candid[SNPNUM];
float cor;
FILE *gf, *sf, *pf, *rf;
clock_t start, stop, fs, fe, ce, ft=0, ct=0;
double duration;
long candcount = 0, matchcount = 0;;


void Norm(float *data){

	float interm;
	float offset, scale;
	float avg = 0, sqrsum = 0;

	for (int i = 0; i < VECLEN; i++)
		avg += data[i] / VECLEN;
	for (int i = 0; i < VECLEN; i++){
		data[i] = data[i] / (VECLEN*VECLEN);
		sqrsum += data[i]*data[i];
	}
	scale = sqrt(sqrsum);
	for (int i = 0; i < VECLEN; i++)
		data[i] = data[i] / (scale);
}

float Cor(float *s1, float *s2){
	float res = 0;
	for (int i = 0; i < VECLEN; i++)
		res += s1[i] * s2[i];
	return res;
}

float abstr(float num){
	return num>0 ? num : -num;
}


void InitNorm(){

	start = clock();

	fopen_s(&gf, "Real Data\\G.txt", "r");
	fopen_s(&pf, "Real Data\\geneNorm.txt", "w");
	for (int i = 0; i < GENENUM; i++){
		//fscanf_s(gf, "%s", gene.name);
		//printf("%s\n", gene.name);
		for (int p = 0; p < VECLEN; p++){
			fscanf_s(gf, "%f\t", &(gene.gene[p]));
			//printf("%5.5f ", gene.gene[p]);
		}
		//break;
		fscanf_s(gf, "\n");
		Norm(gene.gene);
//		fprintf_s(pf, "%s ", gene.name);
		for (int p = 0; p < VECLEN; p++)
			fprintf_s(pf, "%f ", gene.gene[p]);
		fprintf(pf, "\n");
		//		printf("gene %d done\n", i);
	}
	fclose(gf);
	fclose(pf);

	fopen_s(&sf, "Real Data\\S.txt", "r");
	fopen_s(&pf, "Real Data\\snpNorm.txt", "w");
	for (int i = 0; i < SNPNUM; i++){
//		fscanf_s(sf, "%s", snps.name);
		for (int p = 0; p < VECLEN; p++){
			fscanf_s(sf, "%f ", &(snps.snp[p]));
		}
		fscanf_s(gf, "\n");
		Norm(snps.snp);
//		fprintf_s(pf, "%s ", snps.name);
		for (int p = 0; p < VECLEN; p++){
			fprintf_s(pf, "%f ", snps.snp[p]);
		}
		fprintf(pf, "\n");
		//		printf("snp %d done\n", i);
	}
	fclose(sf);
	fclose(pf);

	stop = clock();
	duration = (double)(stop - start) / CLOCKS_PER_SEC;
	printf("Norm time: %f", duration);
}

void LoadFile(){
	start = clock();

	fopen_s(&gf, "Real Data\\geneNorm.txt", "r");
	fopen_s(&sf, "Real Data\\snpNorm.txt", "r");
	for (int genid = 0; genid < GENENUM; genid++){
//		fscanf_s(gf, "%s", gene.name);
		for (int p = 0; p < VECLEN; p++){
			fscanf_s(gf, "%f ", &(gen[genid].gene[p]));
		}
		fscanf_s(gf, "\n");
	}
	for (int snpid = 0; snpid < SNPNUM; snpid++){
//		fscanf_s(sf, "%s", snps.name);
		for (int p = 0; p < VECLEN; p++){
			fscanf_s(sf, "%f ", &(snp[snpid].snp[p]));
		}
		fscanf_s(sf, "\n");
	}
	fclose(gf);
	fclose(sf);

	stop = clock();
	duration = (double)(stop - start) / CLOCKS_PER_SEC;
	printf("file read time: %f\n", duration);

}

void filter0(){
	start = clock();
	float acor;
	fopen_s(&pf, "Real Data\\Result98.txt", "w");
	fopen_s(&rf, "Real Data\\Num98.txt", "w");
	for (int g = 0; g < GENENUM; g++){
		fs = clock();
		candnum = 0;
		matchcount = 0;
		for (int s = 0; s < SNPNUM; s++){
			cor = Cor(gen[g].gene, snp[s].snp);
			acor = abstr(cor);
			if (acor < TH&&acor>T2){
				candid[candnum] = s;
				candnum++;
				candcount += candnum;
			}
		}
		
		
		fe = clock();
		ft += fe - fs;
		for (int s1 = 0; s1 < candnum; s1++){
			for (int s2 = s1 + 1; s2 < candnum; s2++){
				for (int j = 0; j < VECLEN; j++){
					snps.snp[j] = snp[candid[s1]].snp[j] + snp[candid[s2]].snp[j];
				}
				Norm(snps.snp);
				cor = Cor(gen[g].gene, snps.snp);
				if (cor > TH){
					matchcount++;
					fprintf(pf, "%d %d %d %5.8f\n", g, candid[s1], candid[s2], cor);
				}
			}
		}
		ce = clock();
		ct += ce - fe;
		duration = (double)(ce - fs) / CLOCKS_PER_SEC ;
		printf("gene: %d, candnum: %ld, matchcount: %ld, time: %f\n", g, candnum, matchcount,duration);
		fprintf(rf, "%d %ld %ld %f\n", g, candnum, matchcount, duration);
	}
	fclose(pf);
	fclose(rf);

	stop = clock();

	printf("candcount: %ld\n", candcount);
	duration = (double)(stop - start) / CLOCKS_PER_SEC;
	printf("total time: %f\n", duration);
	duration = (double)ft / CLOCKS_PER_SEC;
	printf("filter time: %f\n", duration);
	duration = (double)ct / CLOCKS_PER_SEC;
	printf("process time: %f\n", duration);
}

void Evaluate(){
	FILE *pf,*ef,*f96,*f97;
	char info[255] = { "\0" };
	int lg = 0, rg = 0, rs1 = 0, rs2 = 0,gcount=0;
	float cor, countsum95 = 0, countsum96 = 0, countsum97 = 0;
	long count95 = 0, count96 = 0, count97 = 0;

	fopen_s(&pf, "Real Data\\result.txt", "r");
	fopen_s(&ef, "Real Data\\evaluate.txt", "w");
	fopen_s(&f96, "Real Data\\result96.txt", "w");
	fopen_s(&f97, "Real Data\\result97.txt", "w");

	while (pf != NULL && !feof(pf))
	{
		fscanf_s(pf, "%d %d %d %f\n", &rg, &rs1, &rs2, &cor);
		if (rg != lg){
			gcount++;
			printf("gene %d completed\n", lg);
			fprintf(ef, "%d %d %d %d %d\n", lg, count95, count96, count97);
			lg = rg;
			count95 = 0;
			count96 = 0;
			count97 = 0;
		}
		count95++;
		if (cor > 0.96){
			count96++;
			fprintf(f96, "%d %d %d %f\n", rg, rs1, rs2, cor);
		}
			
		if (cor > 0.97)
		{
			count97++;
			fprintf(f97, "%d %d %d %f\n", rg, rs1, rs2, cor);
		}
		countsum95 += (float)count95 / GENENUM;
		countsum96 += (float)count96 / GENENUM;
		countsum97 += (float)count97 / GENENUM;
	}
	printf("gencount: %d\n", gcount);
	printf("countsum95: %5.4f K\n", countsum95);
	printf("countsum96: %5.4f K\n", countsum95);
	printf("countsum97: %5.4f K\n", countsum95);
	fclose(pf);
	fclose(ef);
	fclose(f96);
	fclose(f97);
}

void main(){

//	InitNorm();
//	LoadFile();
//	filter0();
//	getchar();
	Evaluate();
	getchar();
}
