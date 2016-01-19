#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define GENENUM 10 //(SEEDNUM*GENEPERSEED)
#define SNPPERGENE 5
#define SNPNUM 100//(GENENUM*SNPPERGENE*2)
#define VECLEN 15

#define TH 0.9
#define SQR2 1.414

using namespace std;

struct Gene{
public:
	float gene[VECLEN];
	float offset;
	float scale;
};
struct Snp{
public:
	float snp[VECLEN];
	int gene;
	int peer;
	float offset;
	float scale;
};
struct cgs{
public:
	float cor[SNPNUM][GENENUM];
};
struct css{
public:
	float cor[SNPNUM][GENENUM];
};
struct Relation{
public:
	int source[SNPNUM];
	int peer[SNPNUM];
};
struct EvaRes{
public:
	float trupos=0, truneg=0, falpos=0, falneg=0;
	int pairs;
};

class FileHandler{
public:
	void LoadGene(Gene *data[]){
		FILE *pf;
		*data = (Gene*)malloc(GENENUM*sizeof(Gene));
		fopen_s(&pf, "gene.txt", "r");
		for (int i = 0; i < GENENUM; i++){
			for (int j = 0; j < VECLEN; j++){
				fscanf_s(pf, "%f", &((*data)[i].gene[j]));		
			}
			fscanf_s(pf, "\n");
		}
		fclose(pf);
	}

	void LoadSNP(Snp *data[]){
		FILE *snpf,*sgf;
		*data = (Snp*)malloc(SNPNUM*sizeof(Snp));
		fopen_s(&snpf, "snp.txt", "r");
		fopen_s(&sgf, "snpgen.txt", "r");
		for (int i = 0; i < SNPNUM; i++){
			for (int j = 0; j < VECLEN; j++){
				fscanf_s(snpf, "%f", &((*data)[i].snp[j]));
			}
			fscanf_s(sgf, "%d\t%d", &((*data)[i].gene), &((*data)[i].peer));
			fscanf_s(snpf, "\n");
		}
		fclose(snpf);
		fclose(sgf);
	}

	void WriteGene_Norm(Gene data[]){
		FILE *pf;
		fopen_s(&pf, "gene_n.txt", "w");
		for (int i = 0; i < GENENUM; i++){
			fprintf_s(pf, "%5.5f\t%5.5f\n", data[i].offset, data[i].scale);
			for (int j = 0; j < VECLEN; j++){
				fprintf_s(pf, "%5.5f\t", data[i].gene[j]);
			}
			fprintf_s(pf, "\n");
		}
		fclose(pf);
	}

	void WriteSNP_Norm(Snp data[]){
		FILE *pf;
		fopen_s(&pf, "snp_sum_n.txt", "w");
		for (int i = 0; i < SNPNUM; i++){
			fprintf_s(pf, "%d\t%d\t%5.5f\t%5.5f\n", data[i].gene, data[i].peer, data[i].offset, data[i].scale);
			for (int j = 0; j < VECLEN; j++){
				fprintf_s(pf, "%5.5f\t", data[i].snp[j]);
			}
			fprintf_s(pf, "\n");
		}
		fclose(pf);
	}
	
	void WriteCorGeneSnp(float* corgenesnp){
		FILE *pf;
		fopen_s(&pf, "CorSnpSnp.txt", "w");
		for (int i = 0; i < SNPNUM; i++){
			int offset = i*SNPNUM;
			for (int j = 0; j < SNPNUM; j++){
				printf("snpid %d\tsnpid%d\n", i, j);
				fprintf_s(pf, "%5.5f ", corgenesnp[offset+j]);
			}
			fprintf_s(pf, "\n");
		}
		fclose(pf);
	}

	void LoadCorGeneSnp(cgs *corgenesnp){
		FILE *pf;
		fopen_s(&pf, "CorGeneSnp.txt", "r");
//		corgenesnp = (cgs*)malloc(sizeof(cgs));
		for (int i = 0; i < SNPNUM; i++){
//			int offset = i*GENENUM;
			for (int j = 0; j < GENENUM; j++){
//				printf("snpid %d\tgeneid%d\n", i, j);
				fscanf_s(pf, "%f ", &(corgenesnp->cor[i][j]));
			}
			fscanf_s(pf, "\n");
		}
		fclose(pf);
	}
	
	void WriteFilterResult(char* filename,cgs corsg){
		FILE *pf;
		char str[255] = "\0";
		fopen_s(&pf, filename, "a");
		for (int i = 0; i < SNPNUM; i++){
			for (int j = 0; j < GENENUM; j++){
				sprintf_s(str,sizeof(str),"%5.5f ", corsg.cor[i][j]);
				fprintf_s(pf, str);
			}
			fprintf_s(pf, "\n");
		}
		fclose(pf);
	}

	void LoadCorSnpSnp(css *corsnpsnp){
		FILE *pf;
		fopen_s(&pf, "CorSnpSnp.txt", "r");
		//		corgenesnp = (cgs*)malloc(sizeof(cgs));
		for (int i = 0; i < SNPNUM; i++){
			//			int offset = i*GENENUM;
			for (int j = 0; j < SNPNUM; j++){
//				printf("snpid %d\tsnpid%d\n", i, j);
				fscanf_s(pf, "%f ", &(corsnpsnp->cor[i][j]));
			}
			fscanf_s(pf, "\n");
		}
		fclose(pf);
	}

	void LoadRelation(Relation *re){
		FILE *pf;
		fopen_s(&pf, "snpgen.txt", "r");
		for (int i = 0; i < SNPNUM; i++){
			fscanf_s(pf, "%d %d", &(re->source[i]), &(re->peer[i]));
			fscanf_s(pf, "\n");
		}
		fclose(pf);
	}

	void WriteRes(char *filename, EvaRes *res){
		FILE *pf;
		fopen_s(&pf, filename, "w");
		for (int i = 0; i < 20; i++){
			fprintf_s(pf, "%5.5f %5.5f %5.5f %5.5f %d\n", 
					  res[i].trupos, res[i].truneg, res[i].falpos, res[i].falneg, res[i].pairs);
		}
		fclose(pf);
	}
};

class Calc{
public:
	void Norm(float *data, float *offset, float *scale){
		float avg=0, sqrsum = 0;
		for (int i = 0; i < VECLEN; i++)
			avg += data[i]/VECLEN;
		*offset = avg;
		for (int i = 0; i < VECLEN; i++)
			sqrsum += (data[i] - avg)*(data[i] - avg);
		*scale = sqrt(sqrsum);
		for (int i = 0; i < VECLEN; i++)
			data[i] = (data[i]-avg)/(*scale);
	}

	float Cor(float *s1, float *s2){
		float res=0;
		for (int i = 0; i < VECLEN; i++){
			res += s1[i] * s2[i];
		}
		return res;
	}

};

void TablesGenerator(){
	/*
	Gene *gens=NULL;
	Snp *snps=NULL;
	Snp *snpsum = (Snp*)malloc(SNPNUM/2*sizeof(Snp));
	float corGeneSnp[SNPNUM/2][GENENUM] = { 0 };
	float corSnpSnp[SNPNUM][SNPNUM] = { 0 };
	Calc Cal;
	FileHandler FHandler;
	FHandler.LoadGene(&gens);
	FHandler.LoadSNP(&snps);

	printf("loaddata\n");
	for (int i = 0; i < VECLEN; i++)
	printf("%5.2f\t", gens[0].gene[i]);
	printf("\nnormalized:\n");
	Cal.Norm(gens[0].gene, &(gens[0].offset), &(gens[0].scale));
	for (int i = 0; i < VECLEN; i++){
	printf("%2.5f\t", gens[0].gene[i]);
	if (i % 5 == 0 && i != 0)
	printf("\n");
	}

	for (int i = 0; i < GENENUM; i++)
	Cal.Norm(gens[i].gene, &(gens[i].offset), &(gens[i].scale));
	for (int i = 0; i < SNPNUM; i++)
	Cal.Norm(snps[i].snp, &(snps[i].offset), &(snps[i].scale));
	FHandler.WriteGene_Norm(gens);
	FHandler.WriteSNP_Norm(snps);
	for (int i = 0; i < SNPNUM; i++)
	for (int j = 0; j < GENENUM; j++)
	corGeneSnp[i][j] = Cal.Cor(gens[j].gene, snps[i].snp);
	FHandler.WriteCorGeneSnp(&corGeneSnp[0][0]);

	for (int i = 0; i < GENENUM; i++){

	Cal.Norm(gens[i].gene, &(gens[i].offset), &(gens[i].scale));
	}
	for (int i = 0; i < SNPNUM/2; i++){
	for (int j = 0; j < VECLEN; j++){
	printf("%d,%d,%d\n", i, SNPNUM - i - 1,j);
	Cal.Norm(snps[i].snp, &(snps[i].offset), &(snps[i].scale));
	Cal.Norm(snps[SNPNUM - i - 1].snp, &(snps[SNPNUM - i - 1].offset), &(snps[SNPNUM - i - 1].scale));
	snpsum[i].snp[j] = snps[i].snp[j] + snps[SNPNUM - i - 1].snp[j];
	}
	Cal.Norm(snpsum[i].snp, &(snpsum[i].offset), &(snpsum[i].scale));
	}
	FHandler.WriteGene_Norm(gens);
	FHandler.WriteSNP_Norm(snpsum);
	for (int i = 0; i < SNPNUM/2; i++)
	for (int j = 0; j < GENENUM; j++)
	corGeneSnp[i][j] = Cal.Cor(gens[j].gene, snpsum[i].snp);
	FHandler.WriteCorGeneSnp(&corGeneSnp[0][0]);

	for (int i = 0; i < SNPNUM; i++)
	Cal.Norm(snps[i].snp, &(snps[i].offset), &(snps[i].scale));
	FHandler.WriteSNP_Norm(snps);
	for (int i = 0; i < SNPNUM; i++)
	for (int j = 0; j < SNPNUM; j++){
	printf("%d,%d\n", i, j);
	corSnpSnp[i][j] = Cal.Cor(snps[i].snp, snps[j].snp);
	}
	FHandler.WriteCorGeneSnp(&corSnpSnp[0][0]);
	*/
}

class Filter{
public:
	void Filter0(cgs* res,cgs cor, float thres){
		for (int i = 0; i < SNPNUM; i++){
			for (int j = 0; j < GENENUM; j++){
				if (cor.cor[i][j] < thres)
					res->cor[i][j] = cor.cor[i][j];
				else
					res->cor[i][j] = 0;
			}
		}
	}

	void Filter1(cgs *res,cgs cor, float thr){
		float thres = thr*(SQR2 - 1);
		for (int i = 0; i < SNPNUM; i++){
			for (int j = 0; j < GENENUM; j++){
				if (cor.cor[i][j] >= thres)
					res->cor[i][j] = cor.cor[i][j];
				else
					res->cor[i][j] = 0;
			}
		}
	}

	void Filter2(cgs *res, cgs cor, float thres){
		float thbase = SQR2*thres;
		for (int i = 0; i < SNPNUM; i++){
			for (int j = 0; j < GENENUM; j++){

				if (cor.cor[i][j] == 0){
					for (int k = 0; k < SNPNUM; k++){
						res[i].cor[k][j] = 0.0;
					}
					continue;
				}

				for (int k = 0; k < SNPNUM; k++){
					if (cor.cor[k][j] > thbase - cor.cor[i][j] && i != k)
						res[i].cor[k][j] = cor.cor[k][j];
					else 
						res[i].cor[k][j] = 0.0;
				}
				
				
			}
			/*
			if (i == 0 && thres <0.805){
				for (int x = 0; x < SNPNUM; x++)
					for (int y = 0; y < GENENUM; y++){
					printf("%5.5f\t", res[i].cor[x][y]);
					}
				printf("\n");

			}*/
		}
	}

	EvaRes UncondEvaluator(cgs cor, Relation re){
		EvaRes res;
		int tp = 0, tn = 0, fp = 0, fn = 0, pairs = 0;
		int nonzerocount = 0;
		int posspace = SNPNUM*1, negspace =SNPNUM*(GENENUM-1);
		for (int j = 0; j < GENENUM; j++){
			nonzerocount = 0;
			for (int i = 0; i < SNPNUM; i++){
				if (cor.cor[i][j] != 0){
					nonzerocount++;
					if (re.source[i] == j)
						tp++;
					else
						fp++;
				}
				else {
					if (re.source[i] == j)
						fn++;
					else
						tn++;
				}
			}
			pairs += nonzerocount*(nonzerocount - 1) / 2;
		}
//		printf("%d %d %d %d %d\n",
//			tp, tn, fp, fn, pairs);
		res.trupos = (float)tp / posspace;
		res.truneg = (float)tn / negspace;
		res.falpos = (float)fp / negspace;
		res.falneg = (float)fn / posspace;
		res.pairs = pairs;
		return res;
	}

	EvaRes CondEvaluator(cgs *cor, Relation re){ 
		EvaRes res;
		int tp = 0, tn = 0, fp = 0, fn = 0, pairs = 0;
		int nonzerocount = 0;
		int posspace = SNPNUM*9, negspace = (SNPNUM*GENENUM-9)*SNPNUM;
		for (int i = 0; i < SNPNUM; i++){
			for (int j = 0; j < GENENUM; j++){
				nonzerocount = 0;
				for (int k = 0; k < SNPNUM; k++){
					if (re.source[i] == j&&re.source[k] == j){
						if (cor[i].cor[k][j] == 0)
							fn++;
						else{
							nonzerocount++;
							tp++;
						}
					}
					else{
						if (cor[i].cor[k][j] == 0)
							tn++;
						else{
							if (i != k)
								nonzerocount++;
							fp++;
						}
					}
				}
				pairs += nonzerocount;
			}
		}
		printf("%d %d %d %d %d\n",
			tp, tn, fp, fn, pairs);
		res.trupos = (float)tp / posspace;
		res.truneg = (float)tn / negspace;
		res.falpos = (float)fp / negspace;
		res.falneg = (float)fn / posspace;
		res.pairs = pairs;
		return res;
	}
};


void miao(){
	cgs corgensnp,filtered0,filtered1,filtered2[SNPNUM];


	FileHandler fh;
	Filter fil;
	Relation re;
	EvaRes res0[20],res1[20],res2[20];
	float thres[20];

	fh.LoadCorGeneSnp(&corgensnp);
	fh.LoadRelation(&re);
	/*
	for (int i = 0; i < SNPNUM; i++){
		printf("%d,%d\n", re.source[i], re.peer[i]);
	}
*/
	for (int i = 0; i < 20; i++){
		thres[i] = 0.8 + i*0.01;
		printf("%2.4f ", thres[i]);
	}
//	printf("\n");
		
		

	for (int i = 0; i < 20; i++){
		fil.Filter0(&filtered0, corgensnp, thres[i]);
		res0[i]=fil.UncondEvaluator(filtered0, re);
	}
	fh.WriteRes("F0-stat.txt", res0);

//	fh.WriteFilterResult("F0-0.9.txt", filtered);
	
	for (int i = 0; i < 20; i++){
		fil.Filter1(&filtered1, filtered0, thres[i]);
		res1[i] = fil.UncondEvaluator(filtered1, re);
	}
	fh.WriteRes("F1-stat.txt", res1);
	
//	fh.WriteFilterResult("F1-0.9.txt", filtered);

	for (int i = 0; i < 20; i++){
		fil.Filter2(filtered2, filtered1, thres[i]);
		res2[i] = fil.CondEvaluator(filtered2, re);
	}
	fh.WriteRes("F2-stat.txt", res2);

//	fil.Filter2(filtered, corgensnp);
//	fh.WriteFilterResult("F2-0.9.txt", filtered);




	getchar();
}
