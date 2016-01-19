#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define GENENUM 200 //(SEEDNUM*GENEPERSEED)
#define SNPPERGENE 10
#define SNPNUM 4000//(GENENUM*SNPPERGENE*2)
#define VECLEN 50

#define TH 0.9
#define SQR2 1.414
#define T2 0.3736

using namespace std;

struct Gene{
	float gene[VECLEN];
};
struct Snp{
	float snp[VECLEN];
	int gene = 0;
	int peer = 0;
};

void Norm(float *data){
	float offset, scale;
	float avg = 0, sqrsum = 0;
	for (int i = 0; i < VECLEN; i++)
		avg += data[i] / VECLEN;
	offset = avg;
	for (int i = 0; i < VECLEN; i++)
		sqrsum += (data[i] - avg)*(data[i] - avg);
	scale = sqrt(sqrsum);
	for (int i = 0; i < VECLEN; i++)
		data[i] = (data[i] - avg) / (scale);
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


void sp2(){
	printf("start\n");
	FILE *gf, *sf, *pf;
	Gene gene,gen[GENENUM];
	Snp snps,snpsum,snp[SNPNUM];
	int candnum = 0, candid[SNPNUM];
	long count=0;
	float cor = 0, acor = 0;
	clock_t start, fs, fe, re, fil = 0, run = 0, end, vs, ve, vt = 0;
	double  duration;



	start = clock();

	fopen_s(&gf, "data\\geneNorm.txt", "r");
	fopen_s(&sf, "data\\snpNorm.txt", "r");
	fopen_s(&pf, "data\\result2.txt", "w");
	for (int genid = 0; genid < GENENUM; genid++){
		for (int p = 0; p < VECLEN; p++){
			fscanf_s(gf, "%f ", &(gen[genid].gene[p]));
		}
		fscanf_s(gf, "\n");
	}
	for (int snpid = 0; snpid < SNPNUM; snpid++){
		for (int p = 0; p < VECLEN; p++){
			fscanf_s(sf, "%f ", &(snp[snpid].snp[p]));
		}
		fscanf_s(sf, "\n");
	}
	fclose(gf);
	fclose(sf);

	end = clock();
	duration = (double)(end - start) / CLOCKS_PER_SEC;
	printf("file read time: %f\n", duration);

	for (int genid = 0; genid < GENENUM; genid++){
		fs = clock();
		candnum = 0;
		for (int snpid = 0; snpid < SNPNUM; snpid++){
			cor = Cor(gen[genid].gene, snp[snpid].snp);
			acor = abstr(cor);
			if (acor < TH && acor > T2){
				candid[candnum] = snpid;
				candnum++;
			}
		}

		fe = clock();
		fil += fe - fs;
		count+=(candnum-1)*candnum/2;
		printf("gene: %d, candnum: %ld\n", genid, candnum);
		for (int j = 0; j < candnum; j++){
			for (int n = j + 1; n < candnum; n++){
				
				for (int m = 0; m < VECLEN; m++){
					snpsum.snp[m] = snp[candid[j]].snp[m] + snp[candid[n]].snp[m];
				}
				Norm(snpsum.snp);
				cor = Cor(gen[genid].gene, snpsum.snp);
				if (cor > TH){
					fprintf(pf, "%d %d %d %5.8f\n", genid, candid[j], candid[n], cor);
				}
			}
		}
		re = clock();
		run += re - fe;
	}

	fclose(pf);
	printf("total pairs tried: %ld\n", count);

	end = clock();
	duration = (double)(end - start) / CLOCKS_PER_SEC;
	printf("total time: %f\n", duration);
	duration = (double)fil / CLOCKS_PER_SEC;
	printf("filter time: %f\n", duration);
	duration = (double)run / CLOCKS_PER_SEC;
	printf("process time: %f\n", duration);
	

	/*
	FILE *gf, *sf, *pf;
	Gene gen;
	Snp snp,cand[SNPNUM];
	int candnum = 0, candid[SNPNUM];
	float cor=0,acor=0;
	clock_t start,fs,fe,re,fil=0, run=0,end,vs,ve,vt=0;
	double  duration;

	start = clock();

	fopen_s(&gf, "data\\gene.txt", "r");
	fopen_s(&pf, "data\\geneNorm.txt", "w");
	for (int i = 0; i < GENENUM; i++){
		for (int p = 0; p < VECLEN; p++)
			fscanf_s(gf, "%f ", &(gen.gene[p]));
		fscanf_s(gf, "\n");
		Norm(gen.gene);
		for (int p = 0; p < VECLEN; p++)
			fprintf_s(pf, "%f ", gen.gene[p]);
		fprintf(pf, "\n");
	}
	fclose(gf);
	fclose(pf);

	fopen_s(&sf, "data\\snp.txt", "r");
	fopen_s(&pf, "data\\snpNorm.txt", "w");
	for (int i = 0; i < SNPNUM; i++){
		for (int p = 0; p < VECLEN; p++){
			fscanf_s(sf, "%f ", &(snp.snp[p]));
		}
		fscanf_s(gf, "\n");
		Norm(snp.snp);
		for (int p = 0; p < VECLEN; p++){
			fprintf_s(pf, "%f ", snp.snp[p]);
		}
		fprintf(pf, "\n");
	}
	fclose(sf);
	fclose(pf);

	end = clock();
	duration = (double)(end - start) / CLOCKS_PER_SEC;
	printf("Norm time: %f\n", duration);

	fopen_s(&gf, "data\\geneNorm.txt", "r");
	fopen_s(&pf, "data\\result.txt", "w");

	for (int genid = 0; genid < GENENUM; genid++){
		fs = clock();
		candnum = 0;
		vs = clock();
		for (int p = 0; p < VECLEN; p++){
			fscanf_s(gf, "%f ", &(gen.gene[p]));
		}
		fscanf_s(gf, "\n");
		ve = clock();
		vt += ve - vs;
		fopen_s(&sf, "data\\snpNorm.txt", "r");
		for (int snpid = 0; snpid < SNPNUM; snpid++){
			vs = clock();
			for (int p = 0; p < VECLEN; p++){
				fscanf_s(sf, "%f ", &(snp.snp[p]));
			}
			ve = clock();
			vt += ve - vs;
			fscanf_s(sf, "\n");
			cor = Cor(gen.gene, snp.snp);
			acor = abstr(cor);
			if (acor < TH && acor > T2){
				for (int p = 0; p < VECLEN; p++){
					cand[candnum].snp[p] = snp.snp[p];
				}
				candid[candnum] = snpid;
				candnum++;
			}
		}
		fclose(sf);

		fe = clock();
		fil += fe - fs;

		for (int j = 0; j < candnum; j++){
			for (int n = j + 1; n < candnum; n++){
				for (int m = 0; m < VECLEN; m++){
					snp.snp[m] = cand[j].snp[m] + cand[n].snp[m];
				}
				Norm(snp.snp);
				cor = Cor(gen.gene, snp.snp);
				if (cor > TH){
					fprintf(pf, "%d %d %d %5.8f\n", genid, candid[j], candid[n], cor);
				}
			}
		}
		re = clock();
		run += re-fe;
	}

	fclose(gf);
	fclose(pf);

	end = clock();
	duration = (double)(end - start) / CLOCKS_PER_SEC;
	printf("total time: %f\n", duration);
	duration = (double)fil / CLOCKS_PER_SEC;
	printf("filter time: %f\n", duration);
	duration = (double) run/ CLOCKS_PER_SEC;
	printf("process time: %f\n", duration);
	duration = (double)vt / CLOCKS_PER_SEC;
	printf("file read time: %f\n", duration);
	*/
	
	

/*
	FILE *pf, *ef;
	char info[255] = { "\0" };
	int lg = 0, ls = -1, rg=0, rs1=0, rs2=0;
	int ttp = 0, tfn = 0, tfp = 0, tc = 0, tp = 0, fn = 0, fp = 0, c = 0;
	float cor;
	fopen_s(&pf, "data\\result.txt", "r");
	fopen_s(&ef, "data\\evaluate.txt", "w");

	while (pf != NULL && !feof(pf))
	{
		fscanf_s(pf, "%d %d %d %f\n", &rg, &rs1, &rs2, &cor);
		if (rg != lg){
			printf("gene %d completed\n", lg);
			fprintf(ef, "%d %d %d %d %d\n", lg, c, tp, fp, fn);
			lg = rg; ls = lg*20-1;
			ttp += tp; tfn += fn; tfp += fp; tc += c;
			tp = 0; fn = 0; fp = 0; c = 0;
		}
		c++;
		if ((rs1 >= rg * 20 || rs2 < (rg + 1) * 20) && (rs2 == rs1 + 1) && (rs1%2 == 0)){
			tp++;
			if (rs1 > ls+1){
				fn += (rs1 - ls) / 2;
				//printf("gid:%d snpid: %d rs1:%d ls:%d\n",lg, rs1, ls);
				//system("pause");
			}
			ls = rs2;
		}
		else
			fp++;

//		fscanf_s(pf, "%s\n", info);

	}
	fprintf(ef, "%d %d %d %d %d\n", lg, c, tp, fp, fn);
	lg = rg; ls = 0;
	ttp += tp; tfn += fn; tfp += fp; tc += c;
	printf("num : %d\n", tc);
	printf("tp : %d\n", ttp);
	printf("fn : %d\n", tfn);
	printf("fp : %d\n", tfp);

	fclose(pf);
	fclose(ef);

	*/


	getchar();
}
