#include <stdio.h>
#include <iostream>
#include <x86intrin.h>
#include <fstream>
#include "counter.h"
using namespace std;

#define __STDC_LIMIT_MACROS    1
#define MAXROW 50
#define MAXCOL 100
#define DEAD 0
#define ALIVE 1
#define SUMTIME 500000

float map[MAXROW][MAXCOL],newmap[MAXROW][MAXCOL],simdmap[MAXROW][MAXCOL],neighborTable[MAXROW][MAXCOL];
int init();
int calSIMD();
void evolutionSIMD();
int neighbors(int, int);
void evolution();
void outputMap();
void copyMap();

int main()
{
  init();
  cout<<"SIMD enabled"<<endl;
  startTiming();  
  evolutionSIMD();
  stopWithPrintTiming();


  init();
  cout<<"serilized "<<endl;
  startTiming();  
  evolution();
  stopWithPrintTiming(); 

  return 0;
}
 


int init()
{
  int row, col;
  string line; 
  fstream in("live.txt",ios::in);

  if(!in)
  {
  cout<<"error"<<endl;
  }

  row=0;
  col=0;
  while(!in.eof()) 
  { 
    getline(in,line);
    col=0;
    for(int i = 0 ;i < line.length() ; i++) {
      if(line[i]=='0'){
            map[row][col] = DEAD;
            col++;
      }
      else if(line[i]=='1')
      {     
            map[row][col] = ALIVE;
            col++;
      }
      }

    row++;
    
  }

  return 0;
}


 
void evolutionSIMD(){
  int row, col;
  int iterateTime=0;

  while (iterateTime<SUMTIME)
  {
    outputMap();
    calSIMD();
    for (row = 0; row < MAXROW; row++)
    {
      for (col = 0; col < MAXCOL; col++)
      { 
        switch (int(neighborTable[row][col]))
        {
          case 0:
          case 1:
          case 4:
          case 5:
          case 6:
          case 7:
          case 8:
            newmap[row][col] = DEAD;
            break;
          case 2:
            newmap[row][col] = map[row][col];
            break;
          case 3:
            newmap[row][col] = ALIVE;
            break;
        }
      }
    }
    copyMap();
    iterateTime++;
  }
}


int calSIMD()
{
  // 32int*4=128 
  int nBlockWidth = 4;
  int cntBlock = MAXROW*MAXCOL/nBlockWidth;
  int cntRem = MAXROW*MAXCOL%nBlockWidth;
  __m128  top1;
  __m128  middle1;
  __m128  bottom1;

  float *firstRow=&map[1][0];
  float *lastRow=&map[MAXROW-1][0];
  float *pmap=&map[0][0];
  float *pSimd=&simdmap[0][0];

  for (int i = 0; i < cntBlock; i++){
    //load
    if(pmap<firstRow){
      middle1= _mm_load_ps(pmap);
      bottom1=_mm_load_ps(pmap+MAXCOL);
      middle1=_mm_add_ps(bottom1, middle1);
    }else if(pmap>=lastRow){
      top1=_mm_load_ps(pmap-MAXCOL);
      middle1=_mm_load_ps(pmap);
      middle1=_mm_add_ps(top1, middle1);

    }else{
      top1=_mm_load_ps(pmap-MAXCOL);
      middle1=_mm_load_ps(pmap);
      bottom1=_mm_load_ps(pmap+MAXCOL);
      middle1=_mm_add_ps(top1, middle1);
      middle1=_mm_add_ps(middle1, bottom1);
    }
    //add 

    const float *sumM = (const float*)&middle1;

    for(int i=0;i<4;i++){
    *(pSimd+i)=sumM[i];
    }
    pSimd += nBlockWidth;
    pmap  += nBlockWidth;
  }


    //handle remain 
    for(int i=0; i<cntRem; i++)
    {
      pSimd[i]=pmap[i]+pmap[i-MAXCOL];
    }

    //for col
    memset(neighborTable, 0, sizeof(neighborTable));
    for(int i=0;i<MAXROW;i++){
      for(int j=0;j<MAXCOL;j++){
        neighborTable[i][j]=simdmap[i][j];
        if(j==0){
          neighborTable[i][j]+=simdmap[i][j+1]-map[i][j];
      }else if(j==MAXCOL-1){
          neighborTable[i][j]+=simdmap[i][j-1]-map[i][j];
      }else{
          neighborTable[i][j]+=simdmap[i][j+1]+simdmap[i][j-1]-map[i][j];
      }
      }
    }
    return 0;

}
 void evolution(){
  int row, col;
  int iterateTime=0;
  while (iterateTime<SUMTIME)
  {
    //outputMap();
    for (row = 0; row < MAXROW; row++)
    {
      for (col = 0; col < MAXCOL; col++)
      { 
        switch (neighbors(row, col))
        {
          case 0:
          case 1:
          case 4:
          case 5:
          case 6:
          case 7:
          case 8:
            newmap[row][col] = DEAD;
            break;
          case 2:
            newmap[row][col] = map[row][col];
            break;
          case 3:
            newmap[row][col] = ALIVE;
            break;
        }
      }
    }
    copyMap();
    iterateTime++;
  }
}


int neighbors(int row, int col)
{
  float count = 0.0;
  int c, r;
  for (r = row - 1; r <= row + 1; r++)
  for (c = col - 1; c <= col + 1; c++)
  {
    if (r < 0 || r >= MAXROW || c < 0 || c >= MAXCOL)
      continue;
    if (map[r][c] == ALIVE)
      count=count+1.0;
  }
  if (map[row][col] == ALIVE)
    count=count-1.0;
  return int(count);
}

void outputMap()
{
  int row, col;
  printf("\n\nGame of life cell status\n");
  for (row = 0; row < MAXROW; row++)
  {
    printf("\n%c", ' ');
    for (col = 0; col < MAXCOL; col++)
      if (map[row][col] == ALIVE)
        putchar('#');
      else
        putchar('-');
  }
}


void copyMap()
{
  int row, col;
  for (row = 0; row < MAXROW; row++)
    for (col = 0; col < MAXCOL; col++)
      map[row][col] = newmap[row][col];
}
