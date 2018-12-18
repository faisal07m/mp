#include <iostream>
#include<conio.h>
#include<cstdlib>

#include <cmath>
using namespace std; 
  
int main(int argc , char *argv[]) 
{ 
  int n= atoi(argv[1]);
  printf("%d",n);
  double A[n][n+1] ;
  for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            if(i==j){
            
			A[i][j]=(double)n/10;}
            else{
            A[i][j]=(double)rand() / (RAND_MAX + 1.0);	
			 }
			
			
        }
    }
    
    //elimination
    for (int i=0; i<n; i++) {
        A[i][n]=1;
    }
    for (int k=0; k<n; k++) 
    { 
  
        for (int i=k+1; i<n; i++) 
        { 
            double f = A[i][k]/A[k][k]; 
  
            for (int j=k+1; j<=n; j++) 
                A[i][j] -= A[k][j]*f; 
  
            A[i][k] = 0; 
        } 
      
    }
	  
    //back substitution  
      double x[n];  
   for (int i = n-1; i >= 0; i--) 
    { 
        x[i] = A[i][n]; 
  
        for (int j=i+1; j<n; j++) 
        { 
           
            x[i] -= A[i][j]*x[j]; 
        } 
  
        x[i] = x[i]/A[i][i]; 
    } 
  
  
  
    printf("Solution :\n");
	double arr[n]; 
	double max = arr[0]; 
    for (int i=0; i<n; i++) 
        {
		printf("%e\n", x[i]);
        arr[i]=abs(x[i]-1);
        printf("err : %f\n.......", arr[i]);
	}
     
	 for(int i = 1; i<n; i++)
     {
          if(arr[i] > max)
                max = arr[i];
     }

	 
	 printf("Max error %e\n",max); 
	 
    getch();
    return 0; 
}
