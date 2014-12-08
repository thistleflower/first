#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

FILE *f, *fh, *fhh;

float dx;
float n;
float ht;
float k;
float L;
char f1[10], f2[10];
float *h1, *h2, **h;
float kh1, kh2;//точность
float H1=0, H2=0;
int H;
int or1, or2;//ориентация (верхняя/нижняя границы)
int count1, count2;// размер считываемого массива


//int H, h1, h2, ht;
/*
dx - шаг по сетке Х
n - кол-во узлов по х
ht - высота "трубы"
2 массива с нормированными высотами неровнастями
k - коэф доп подразделения

L = dx * (n - 1)
H = h1 + h2 + ht
k * H / L < 7
*/
typedef struct{
	int kind;
	float speed;
	float weight;
	int type;
} cells;
	
void getWeight (int i, float w) {
	switch (i){
		case 1:
			w = 1.0/9.0;
			break;
		case 3:
			w = 1.0/9.0;
			break;
		case 5:
			w = 1.0/9.0;
			break;
		case 7:
			w = 1.0/9.0;
			break;
		case 2:
			w = 1.0/36.0;
			break;
		case 4:
			w = 1.0/36.0;
			break;
		case 6:
			w = 1.0/36.0;
			break;
		case 8:
			w = 1.0/36.0;
			break;
		case 9:
			w = 4.0/9.0;
			break;
	}
}

void getSpeed (int i, float v){
	switch (i) {
		case 1:
			v = c;//(cos [(i-1)pi/4], sin [(i-1)pi/4])* c
			break;
		case 3:
			v = c;
			break;
		case 5:
			v = c;
			break;
		case 7:
			v = c;
			break;
		case 2: 
			v = c*sqrt(2); //(cos [(i-1)pi/4], sin [(i-1)pi/4])* sqrt(2)*c
			break;
		case 4:
			v = c*sqrt(2);
			break;
		case 6:
			v = c*sqrt(2);
			break;
		case 8:
			v = c*sqrt(2);
			break;
		case 9:
			v = 1; // (0, 0)
			break;
	}
} 
float getHx(float *arr, int size){
	float max = arr[0], min = arr[0];
	float h;
	int i = size;
	
	while (i != 0){
		if( max < arr[i]) max = arr[i];
		if( min > arr[i]) min = arr[i];
		i--;
	}

	h = max-min;
	return h;
}

void getH(){
	/* init H1*/
	fh = fopen(f1, "r");
	if ( fh == NULL )
	{
		printf( "Could not open file\n" );
		exit(1);
	} else {
		fscanf(fh, "%d", &or1);
		//printf("or1 = %d\n", or1);
		fscanf(fh, "%f", &kh1);
		//printf("kh1 = %f\n", kh1);
		fscanf(fh, "%d", &count1);
		//printf("count1 = %d\n", count1);
		int i=0;
		if((h1 = malloc(count1*sizeof(float)))==NULL){
			printf("Ошибка при распределении памяти\n");
			exit(1);
		}
		while(!feof(fh)){
			fscanf(fh, "%f", &h1[i]);
			i++;
		}	
		
		H1 = getHx(h1, i-1);
		//printf("H1 = %f \n", H1);	
		printf("\nor1 = %d	kh1 = %f	count1 = %d	H1 = %f\n", or1, kh1, count1, H1);
	}
	fclose(fh);
	
	/*init H2*/
	fhh = fopen(f2, "r");
	if ( fhh == NULL )
	{
		printf( "Could not open file\n" );
		exit(1);
	} else {
		fscanf(fh, "%d", &or2);
		//printf("or2 = %d\n", or2);
		fscanf(fh, "%f", &kh2);
		//printf("kh2 = %f\n", kh2);
		fscanf(fh, "%d", &count2);
		//printf("count2 = %d\n", count2);
		int i=0;
		if((h2 = malloc(count2*sizeof(float)))==NULL){
			printf("Ошибка при распределении памяти\n");
			exit(1);
		}
		while(!feof(fhh)){
			fscanf(fhh, "%f", &h2[i]);
			i++;
		}	
		
		H2 = getHx(h2, i-1);
		//printf("H2 = %f \n", H2);
		printf("\nor2 = %d	kh2 = %f	count2 = %d	H2 = %f\n", or2, kh2, count2, H2);
	}
		
	fclose(fhh);
	

	
	//H = H1 + H2 + ht + 2;
	H = H1 + H2 + ht;
	printf("\nH = %d\n", H);	
}

int init(int argc, char **argv){
	f = fopen( argv[1], "r");
	if ( f == NULL )
	{
		printf( "Could not open file\n" );
		exit(1);
	}
	
	fscanf(f, "%f %f %f %f %s %s", &dx, &n, &ht, &k, f1, f2);
	//printf("%f %f %f %f %s %s\n", dx, n, ht, k, f1, f2);
	
	L = dx * (n - 1);
	printf("L = %f\n", L);
	printf("\ndx = %f	n = %f	 ht = %f	 k = %f\n", dx, n, ht, k);
	
	getH();
	
	/* Условие правильного разбиения */
	if (k*H/dx < 7) {
		printf("Error: k*H/dx < 7 \n");
		exit(1);
	} else {
		printf("\nif DONE \n");
	}	
	
	fclose(f);

	return 0;	
}

void take_array(){
	//int i;
	int y = 0, x = 0;
	if((h = malloc(count1*sizeof(float*)))==NULL){
		printf("Ошибка при распределении памяти в take_array h\n");
		exit(1);
	}
	for(int i=0;i<count1;i++){
		if((*(h+i) = malloc(H*sizeof(float)))==NULL){
			printf("Ошибка при распределении памяти в take_array *h\n");
			exit(1);
		}
	}
	
	for(int i = 0; i < count1; i++){
		for(int j = 0; j < H; j++){
			h[i][j] = 0;
		}
	}

	/*for(int i = 0; i < count1; i++){
		if (or1 == 1) {
			y = (int)round(H2/2.0) - (int)h2[i];
			x = H2 + ht + 1 + (int)round(H1/2.0) + (int)h1[i];
		} else {
			y = (int)round(H1/2.0) - (int)h1[i];
			x = H1 + ht + 1 + (int)round(H2/2.0) + (int)h2[i];
		}
		
		
		h[i][y] = 1;
		h[i][x] = 1;
		
	}*/
	for(int i = 0; i < count1; i++){
		
		if ( or1 == 1) {
			x = (int)round(H2/2.0) - (int)h2[i];
			y = H2 + ht + 1 + (int)round(H1/2.0) + (int)h1[i];
		} else {
			x = (int)round(H1/2.0) - (int)h1[i];
			y = H1 + ht + (int)round(H2/2.0) + (int)h2[i];
		}
		printf("%d\n", x);
		
		for(int j=0; j< H; j++){
			if( j >= y){
				h[i][j] = 1;
			} else if ( j < x ){
				h[i][j] = 2;
			} else {
				h[i][j] = 0;
			}
			
		}
		//printf("\n");
	}
	// output tube
	for(int i = 0; i < count1; i++){
		for(int j = 0; j < H; j++){
			printf("%d", (int)h[i][j]);
		}
		printf("\n");
	}
	

}

int main (int argc, char **argv) {
	init(argc, argv);
	take_array();
	free(h1);
	free(h2);
	free(*h);
	free(h);
	//~ for(int i=0;i<count1;i++){
		//~ free();
	//~ }
	return 0;
}

