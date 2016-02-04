#include<stdio.h>
#include<string.h>
#include<stdlib.h>

extern int Array2048[10][10];
void copyFrom2048ArrayToDrawMessage(int n,int (*drawMsg)[10] ,int (*Array2048)[10]);
void swap(int *a ,int *b);

unsigned int myrandom (void);


void line(int n);
bool isFull(int n);
bool isWin(int n);
bool isOver(int n);

void fillBox(int n);

void up_remove_blank(int n);
void down_remove_blank(int n);
void left_remove_blank(int n);
void right_remove_blank(int n);
void left(int n);
void right(int n);
void up(int n);
void down(int n);
