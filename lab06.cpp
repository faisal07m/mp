#include <iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <omp.h>
#include <cstdlib>
const unsigned char LIVE = 1;
const unsigned char DEAD = 0;

char** c_gen;
char** new_gen;
int COLUMN, ROW;


int get_live_neighbours(int i, int j) 
    {
    int live_neighbor_count= 0;

    if (c_gen[i - 1][j - 1] == LIVE) { live_neighbor_count++; }
    if (c_gen[i - 1][j] == LIVE) { live_neighbor_count++; }
    if (c_gen[i - 1][j + 1] == LIVE) { live_neighbor_count++; } 
    if (c_gen[i][j + 1] == LIVE) { live_neighbor_count++; } 
    if (c_gen[i + 1][j + 1] == LIVE) { live_neighbor_count++; } 
    if (c_gen[i + 1][j] == LIVE) { live_neighbor_count++; } 
    if (c_gen[i + 1][j - 1] == LIVE) { live_neighbor_count++; }   
    if (c_gen[i][j - 1] == LIVE) { live_neighbor_count++; }       

	return live_neighbor_count;
    }

char rules(int i, int j)
{
	int live_neighbors = get_live_neighbours(i, j);

	
	if(live_neighbors  > 3)
		return DEAD;

	else if(live_neighbors < 2)
		return DEAD;

	else if(c_gen[i][j] == LIVE && (live_neighbors  == 2 || live_neighbors  == 3))
		return LIVE;

	else if(c_gen[i][j] == DEAD && live_neighbors == 3)
		return LIVE;
}

void start_game()
{
	int i, j = 0;

    #pragma omp parallel for  private(i,j)
	for (i = 1; i < COLUMN - 1; i++)
	{
		for (j = 1; j < ROW - 1; j++)
		{
			new_gen[i][j] = rules(i, j);
		}
	}
}

void save_new_gen()
{
	int i, j = 0;
       #pragma omp parallel  for  private(i,j)
	for (i = 1; i < COLUMN - 1; i++)
	{
		for (j = 1; j < ROW - 1; j++)
		{
			c_gen[i][j] = new_gen[i][j];
		}
	}
}



void write_ppm(unsigned char* gen, char* filename, size_t side_length)
{

  size_t i;
  FILE *fp = fopen(filename, "wb");
  fprintf(fp, "P6\n%zd %zd\n255\n", side_length, side_length);
  
  const unsigned char _black[] = {0x00, 0x00, 0x00};
  const unsigned char _white[] = {0xff, 0xff, 0xff};
  int a=0,r=0;

  for(i=0; i<side_length*side_length; ++i)
   {
    if( c_gen[a][r]==1)
     {
      fwrite(_black, 1 , 3 , fp);
     }
    else fwrite(_white, 1 , 3 , fp);
    
    r++;
    
    if((i+1)%(side_length)==0){
     	a++;
     	r = 0;
     }}
  fclose(fp);

}

unsigned char* read_ppm(char* filename, size_t* side_length,int k)
{
  
  size_t h = 0;
  int a=0,r=0,i=0;
  FILE *fp = fopen(filename, "rb");
  fscanf(fp, "P6\n%zd %zd\n255\n", side_length, &h);
  
  if (*side_length != h) exit(1);

  unsigned char* gen = (unsigned char *)malloc( (*side_length) * (*side_length) );
 
  COLUMN=*side_length;
  ROW=*side_length;
  c_gen= new char*[COLUMN];
	new_gen = new char*[COLUMN];

	for(int i = 0; i < COLUMN; i++)
	{
		c_gen[i] = new char[ROW];
	}

	for(int i = 0; i < COLUMN; i++)
	{
		new_gen[i] = new char[ROW];
	}
  
  for(i=0;i<(*side_length)*(*side_length);i++)
  {
    unsigned char buff[3];
    fread(buff, 1 , 3 , fp);
   
    if(buff[0] == 0 && buff[1] == 0 && buff[2] == 0){gen[i] = LIVE; c_gen[a][r]=LIVE;} // black pixel
      else {
           c_gen[a][r]=DEAD; 
           gen[i] = DEAD;
           }
	r++;
    
 	if((i+1)%(*side_length)==0){
     
	r = 0;a++;
     
     			
	}}

  fclose(fp);
  return gen;

}

int main(int argc, char* argv[])
{

  size_t side_length;
  char file_in[100],file_out[100],first_file_out[100];
  int i=0,zero=0;
  

  unsigned char * prev = read_ppm(argv[1], &side_length,i);
  sprintf(first_file_out,"output-%d.ppm",zero);
  write_ppm(prev, first_file_out, side_length);
 
  for (i = 1; i <= atoi(argv[2]); i++)
  {
	sprintf(file_in,"output-%d.ppm",i-1);
                
	unsigned char * prev = read_ppm(file_in, &side_length,i);
        sprintf(file_out,"output-%d.ppm",i);
        #pragma omp parallel
	start_game();
	#pragma omp parallel
	save_new_gen();
                  
	write_ppm(prev, file_out, side_length);
	free(prev);
		
	}

 
  

  return 0;

} 
