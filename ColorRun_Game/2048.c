#include <file.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>



int Array2048[10][10]={0};

void swap(int *a ,int *b)
{
	int c = *a;
	*a = *b;
	*b = c;
}
void copyFrom2048ArrayToDrawMessage(int n,int (*drawMsg)[10] ,int (*Array2048)[10])
{
	int i,j;

		for ( i = 0; i < n; ++i){
			for ( j = 0; j < n; ++j){
				drawMsg[i][j]=Array2048[i][j];
//				printf("%d",Array2048[i][j]);
				}
			}

}
unsigned int myrandom (void)
{
   static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
   unsigned int b;
   b  = ((z1 << 6) ^ z1) >> 13;
   z1 = ((z1 & 4294967294U) << 18) ^ b;
   b  = ((z2 << 2) ^ z2) >> 27;
   z2 = ((z2 & 4294967288U) << 2) ^ b;
   b  = ((z3 << 13) ^ z3) >> 21;
   z3 = ((z3 & 4294967280U) << 7) ^ b;
   b  = ((z4 << 3) ^ z4) >> 12;
   z4 = ((z4 & 4294967168U) << 13) ^ b;
   return (z1 ^ z2 ^ z3 ^ z4);
}

void line(int n){
	int i;
	for (i = 0; i < n; ++i){
		printf("--------");
	}
	printf("-");
	printf("\n");
}
bool isFull(int n){
	int i,j;
	bool mark = true;
	for ( i = 0; i < n; ++i){
		for ( j = 0; j < n; ++j){
			if (Array2048[i][j] == 0){
				mark = false;
				return mark;
			}
		}
	}
	return mark;
}
bool isWin(int n){
	int i,j;
	bool mark = false;
	for ( i = 0; i < n; ++i){
		for ( j = 0; j < n; ++j){
			if (Array2048[i][j] >=2048){
				mark = true;
				return mark;
			}
		}
	}
	return mark;
}

bool isOver(int n){
	int i,j;
	if (!isFull(n)){
		return false;
	}
	for ( i = 0; i < n; ++i){
		for ( j = 0; j < n; ++j){
			if (Array2048[i][j] == Array2048[i][j + 1] || Array2048[i][j] == Array2048[i + 1][j]){
				return false;
			}
		}
	}
	return true;
}

void fillBox(int n){
	int i, j, num;

	if (isFull(n)){
		return;
	}

	while (true){
		i = rand() % n;
		j = rand() % n;
		num = rand() % 2 == 0 ? 2 : 4;

		if (Array2048[i][j] == 0){
			Array2048[i][j] = num;
			break;
		}
	}
}

void up_remove_blank(int n){
	int i,j,k;
	for(j=0;j<n;j++){
		for(i=1;i<n;i++){
			k=i;
			while(k-1>=0&&Array2048[k-1][j]==0){//上面的那个为空
				swap(&Array2048[k][j],&Array2048[k-1][j]);
				k--;

			}
		}
	}
}
void down_remove_blank(int n){
	int i,j,k;
	for(j=0;j<n;j++){
		for(i=n-2;i>=0;i--){
			k=i;
			while(k+1<=n-1&&Array2048[k+1][j]==0){//上面的那个为空
				swap(&Array2048[k][j],&Array2048[k+1][j]);
				k++;
			}
		}
	}
}
void left_remove_blank(int n){
	int i,j,k;
	for(i=0;i<n;i++){
		for(j=1;j<n;j++){
			k=j;
			while(k-1>=0&&Array2048[i][k-1]==0){//上面的那个为空
				swap(&Array2048[i][k],&Array2048[i][k-1]);
				k--;
			}
		}
	}
}
void right_remove_blank(int n){
	int i,j,k;
	for(i=0;i<n;i++){
		for(j=n-2;j>=0;j--){
			k=j;
			while(k+1<=n-1&&Array2048[i][k+1]==0){//上面的那个为空
				swap(&Array2048[i][k],&Array2048[i][k+1]);
				k++;
			}
		}
	}
}
void left(int n){
	int i,j;
	for(i=0;i<n;i++){
		for(j=0;j<n-1;j++){
			if(Array2048[i][j]==Array2048[i][j+1]){
				Array2048[i][j]+=Array2048[i][j+1];
				Array2048[i][j+1]=0;
				left_remove_blank(n);
			}
		}
	}
}
void right(int n){
		int i,j;
	for(i=0;i<n;i++){
		for(j=n-1;j>=1;j--){
			if(Array2048[i][j]==Array2048[i][j-1]){
				Array2048[i][j]+=Array2048[i][j-1];
				Array2048[i][j-1]=0;
				right_remove_blank(n);
			}
		}
	}
}
void up(int n){
	int i,j;
	for(j=0;j<n;j++){//每一列
		for(i=0;i<n-1;i++){
			if(Array2048[i][j]==Array2048[i+1][j]){
				Array2048[i][j]=Array2048[i][j]+Array2048[i+1][j];
				Array2048[i+1][j]=0;
				//移除空格
				up_remove_blank(n);
			}
		}
	}
}
void down(int n){
	int i,j;
	for(j=0;j<n;j++){//每一列
		for(i=n-1;i>=1;i--){
			if(Array2048[i][j]==Array2048[i-1][j]){
				Array2048[i][j]=Array2048[i][j]+Array2048[i-1][j];
				Array2048[i-1][j]=0;
				//移除空格
				down_remove_blank(n);
			}
		}
	}
}

/*
int main(){
	int i,j;
	bool mark;
	int n =4;
    fillBox(n);
	fillBox(n);
    output(n);

    printf("%d\n",myrandom() % n);
	printf("%d\n",myrandom() % n);
	printf("%d\n",myrandom() % n);
	printf("%d\n",myrandom() % n);
	printf("%d\n",myrandom() % n);
	printf("%d\n",myrandom() % n);
	printf("%d\n",myrandom() % n);
	printf("%d\n",myrandom() % n);

    while (true){
		char ch;
		scanf("%c%*c", &ch);
		if (ch == 'a'){
			// mark = left(n);
			left_remove_blank(n);
			left(n);
		}
		else if (ch == 'd'){
			// mark = right(n);
			right_remove_blank(n);
			right(n);
		}
		else if (ch == 'w'){
			// mark = up(n);
			up_remove_blank(n);
			up(n);
		}
		else if (ch == 's'){
			// mark = down(n);
			down_remove_blank(n);
			down(n);
		}
		else{
			continue;
		}
		system("clear");
		//printf("Move:\n");
		//output();
		// if (!mark){
		// 	continue;
		// }
		fillBox(n);
		printf("Fill:\n");
		output(n);

		if (isOver(n)){
			printf("\n\nGame Over!\n\n");
			break;
		}
		if (isWin(n))
		{
			printf("\n\n Wow You Win!\n\n");
			break;
			 code
		}
	}

	return 0;
}

*/
