
/*  n = 3000, threads = 40  time in seconds 
-------------------------------------------------------------------------------------------------------------------------------------------
Chunk    			 |  Default   |      2       |       8       |      32      |     128      |  Single Thread (default Chunk)

Static   (Gauss TIME sec) 	  2.224242e+00  2.559708e+00    2.191239e+00   2.334189e+00   2.693574e+00   1.057281e+01 
         (back sub TIME sec)	  9.880314e-02  1.174744e-01    1.055908e-01   9.576066e-02   1.027680e-01   1.470342e-02
         (total TIME sec)	  2.323054e+00  2.677189e+00    2.296837e+00   2.429958e+00   2.796349e+00   1.058752e+01

 
Dynamic   (Gauss TIME sec)  	  2.811208e+00  2.784184e+00    2.666185e+00   2.670090e+00   3.145072e+00   1.059340e+01
          (back sub TIME sec) 	  6.028012e-01  3.655285e-01    1.851279e-01   1.270579e-01   1.157399e-01   6.475342e-02
          (total TIME sec)	  3.414019e+00  3.149723e+00    2.851323e+00   2.797158e+00   3.260819e+00   1.065816e+01

Guided    (Gauss TIME sec)	  2.664658e+00  2.647170e+00    2.665661e+00   2.761532e+00   3.125653e+00   1.055963e+01
          (back sub TIME sec)	  2.839649e-01  2.652135e-01    2.122728e-01   1.625099e-01   1.227327e-01   1.480821e-02
          (total TIME sec) 	  2.948632e+00  2.912393e+00    2.877944e+00   2.924050e+00   3.248396e+00   1.057445e+01

Clearly with 40 threads the program performed alot faster than single threaded computation. Following is the speedup calculation:-

Scheduler (total time) | Multi-Threaded(Default Chunk)   Single Thread(Deafult chunk) | Multi-Threaded Speedup
Static   				 2.323054e+00  <     1.058752e+01       	 8.264466 seconds faster 
Dynamic                 		 3.414019e+00  <     1.065816e+01   		 7.244141 seconds faster  
guided                  		 2.948632e+00  <     1.057445e+01 		 7.625818 seconds faster
------------------------------------------------------------------------------------------------------------------------------------------


*/

#include <iostream>
#include <time.h>
#include<cstdlib>
#include<stdio.h>
#include <cmath>
#include<omp.h>
using namespace std; 
  
int main(int argc , char *argv[]) 
{ 
  int n= atoi(argv[1]);
  int threads = omp_get_max_threads();
  int  chunk ; //atoi(argv[2]);
  //printf("%d",threads);
  //omp_set_num_threads(threads);
  double time1,time2,time3,t_time_s,t_time_e;
  
  double A[n][n+1] ;
  for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {
            if(i==j){
            
		A[i][j]=(double)n/10;
            }
            else{
            unsigned int seed = (unsigned) time(NULL);
	    seed= seed + j;
            A[i][j]=(double)rand_r(&seed)/(RAND_MAX);	
            //printf("\n %lf  rand",A[i][j]);
			 }
			
			
        }
    }
    
    for (int i=0; i<n; i++) {
        A[i][n]=1;
    }
    //printf("\n threads %d \n", omp_get_max_threads());
   //elimination  Ref:- https://en.wikipedia.org/wiki/Gaussian_elimination  without pivot 
    double f=0.0;
    int i=0,j=0;
    t_time_s=omp_get_wtime();
    time1= omp_get_wtime();
    for (int k=0; k<n; k++) 
    { 
        #pragma omp parallel default(none) num_threads(threads) shared(chunk,A,n,k,f) private(i,j) 
        {
        #pragma omp for schedule(static)
        //pragma omp for schedule(dynamic,chunk)
        //#pragma omp for schedule(guided,chunk)
        for (i=k+1; i<n; i++) 
        { 
            f = A[i][k]/A[k][k];
            //put 0 for lower triangle 
            A[i][k] = 0;
            for (j=k+1; j<=n; j++) 
                A[i][j] -= A[k][j]*f; 
  
             
        } 
        }
      
    }
    time2=omp_get_wtime() - time1;
   	  
    //back substitution  
      double x[n];  
   time3= omp_get_wtime();
   #pragma omp parallel default(none) num_threads(threads) shared(A,n,x,chunk) private(i,j) 
   for (int i = n-1; i >= 0; i--) 
    { 
        #pragma omp single
        x[i] = A[i][n]; 
        #pragma omp for  schedule(static)
	//#pragma omp for  schedule(dynamic,chunk)
	//#pragma omp for  schedule(guided,chunk)
        for (int j=i+1; j<n; j++) 
        { 
           
            x[i] -= A[i][j]*x[j]; 
        } 
        #pragma omp single
        x[i] = x[i]/A[i][i]; 
    } 
    double time4 = omp_get_wtime() - time3;
   
    t_time_e = omp_get_wtime() - t_time_s;
   
	double arr[n]; 
	double max = arr[0]; 
    for (int i=0; i<n; i++) 
        {
		
        arr[i]=abs(x[i]-1);
       
	}
     
	 for(int i = 1; i<n; i++)
     {
          if(arr[i] > max)
                max = arr[i];
     }

	 
	printf("Max error in solution %e\n",max); 
 	printf("Time for Gaussian elim : %e seconds\n",time2);
	printf("Time back sub : %e seconds\n",time4);
	printf("Total Time : %e seconds\n",t_time_e);
  

	 
    
    return 0; 
}

