

#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include<string.h>
#include <iomanip>
#include <cmath>
#include <stdlib.h> 
#include <fstream>
#include<sys/time.h>
#define DIGITS 4
using namespace std;

 void *pi_est(void *);

 pthread_mutex_t calculate,store,add;

 double val=0.0, number_of_tosses_so_far=0.0,  tosses_within_circle=0.0,thread_cal_pi;
 int num_threads = 0;
 long num_of_tosses=0;
 int thread=0;
 
 double previous_duration=0.0,  single_thread_duration=0.0;

 const double PI = 3.14159265359;

int main(int argc, char* argv[])
{
        
        ofstream file1,file2;
	
	num_of_tosses = atol(argv[1]);
        num_threads = atoi(argv[2]);
        int arr[2];
        double case1_time_per_thread[num_threads];

        double case1_error_per_thread[num_threads];

        for(int caseP=0;caseP<2;caseP++){
        arr[0]=caseP;
        previous_duration=0.0,  single_thread_duration=0.0;
        cout <<"=========================case-"<<caseP+1<<"==================="<<endl;
        if(caseP==0){
        file1.open("casefirst.dat");
        }
	if(caseP==1){
   	file2.open("caseSecond.dat");
	}
double duration , duration_in_seconds=0.0;
        for(thread = 1; thread <= num_threads ; thread++){

        double speedup_down=0.0;
        tosses_within_circle = 0.0;
        number_of_tosses_so_far=0.0;
	
        val =0.0;
	
	struct timeval start1,end1;
        gettimeofday(&start1,NULL);
	pthread_t threads[thread];
        pthread_mutex_init(&add,NULL);
        
        
        for (int i=0; i < thread; i++)
        {
          arr[1] = i; 
          pthread_create(&threads[i], NULL, pi_est, (void*)arr);
         
        }

        for (int i=0; i < thread; i++)
           {  pthread_join(threads[i], NULL);

              }   
        gettimeofday(&end1,NULL);
	
        duration_in_seconds = (end1.tv_sec - start1.tv_sec)+((end1.tv_usec - start1.tv_usec)/1000000.0);

        double my_value = 4 *tosses_within_circle/number_of_tosses_so_far;
        double my_error = sqrt((PI-my_value)*(PI-my_value));
         if(caseP==0){
           case1_error_per_thread[thread-1] = my_error;
           case1_time_per_thread[thread-1] = duration_in_seconds;
       
         }
        cout << "Threads (" << thread          << ")\n Calculated Value of Pi = " << setprecision(10) << fixed
          << my_value << "\nwith error of " << my_error
          << "\n" << setprecision(0) << fixed
          << num_of_tosses << " Tosses" << "\n";
        if(caseP==1){
           if(my_error > case1_error_per_thread[thread-1]){cout << "Case-2 Thread( "<<thread<<") is less efficient than Case-1 Thread- " <<thread<<endl;}
else if(my_error < case1_error_per_thread[thread-1]){cout << "Case-2 Thread( "<<thread<<") is more efficient than Case-1 Thread- " <<thread<<endl;}
else {cout <<"Thread("<<thread<<") for case-1 and case-2 equally efficient"<<endl;}
           if(duration_in_seconds > case1_time_per_thread[thread-1]){cout << "Case-2 Thread( "<<thread<<") is slower than Case-1 Thread- " <<thread<<endl;}
else if(duration_in_seconds < case1_time_per_thread[thread-1]){cout << "Case-2 Thread( "<<thread<<") is faster than Case-1 Thread- " <<thread<<endl;}
else {cout <<"Thread("<<thread<<")case-1 and case-2 took same amout of time"<<endl;}
         

         }
	
	if(previous_duration != 0 || thread !=1)
	{
	
		if(duration_in_seconds == previous_duration && thread > 2)
		{

		previous_duration = duration_in_seconds;
		cout << "(compared to "<<thread -1<<" Thread) Speed is same : "<< endl;
		}

		else if(duration_in_seconds > previous_duration)
		{
		speedup_down = previous_duration - duration_in_seconds ;
		previous_duration = duration_in_seconds;
		cout << "(compared to "<<thread -1<<" Thread) Speed down : " << fixed << setprecision (DIGITS) 
		     <<  speedup_down << " secs." << endl;

		}
		
		else if(duration_in_seconds < previous_duration)
		{
		speedup_down =  previous_duration - duration_in_seconds ;
		previous_duration = duration_in_seconds;
		cout << "(compared to "<<thread -1<<" Thread) Speed up  : " << fixed << setprecision (DIGITS) 
		     <<  speedup_down << " secs." << endl;

		}
		if(duration_in_seconds == single_thread_duration)
		{
		cout << "(compared to Single Thread)  Speed is same "<< endl;
		}

		else if(duration_in_seconds >  single_thread_duration)
		{
		speedup_down =  single_thread_duration - duration_in_seconds ;
		cout << "(compared to Single Thread) Speed down : " << fixed << setprecision (DIGITS) 
		     <<  speedup_down << " secs." << endl;

		}
		
		else if(duration_in_seconds < single_thread_duration)
		{
		speedup_down =   single_thread_duration - duration_in_seconds ;
		cout << "(compared to Single Thread) Speed up  : " << fixed << setprecision (DIGITS) 
		     <<  speedup_down << " secs." << endl;

		}
	}
        else
 	{        
 		previous_duration = duration_in_seconds;
                single_thread_duration = duration_in_seconds;
	}
 
        cout << "Duration : " << fixed << setprecision (DIGITS) 
             << duration_in_seconds << " secs.\n\n" << endl;
        if(caseP==0){
        file1 <<duration_in_seconds<<"\t"<<thread<<endl;
        }
	if(caseP==1){
        file2 <<duration_in_seconds<<"\t"<<thread<<endl;
        }
	
        }
        }
        file1.close();
        file2.close();
	pthread_exit(NULL);
        pthread_mutex_destroy(&add);

	
	
}

void *pi_est(void* arr)
{
        int *array =(int *)arr;
        int current_thread = array[1];
        double circle_in = 0,circle_out=0;
        double x, y;
       
        unsigned seed = (int) pthread_self();
        for (double n=current_thread; n <= num_of_tosses; n+= thread)
        {
   
           x = (rand_r(&seed)*2)/(double)(RAND_MAX + (-1)) ;
	   y = (rand_r(&seed)*2)/(double)(RAND_MAX + (-1));
           if(array[0]==0){
	   pthread_mutex_lock (&add);
           number_of_tosses_so_far++;
           if((x*x+y*y) <= 1.0 ) {
		tosses_within_circle++;	
        	}
          
		pthread_mutex_unlock (&add);
           }
           else{
 		if((x*x+y*y) <= 1.0 ) {
                  circle_in++;
                }
                
                circle_out++;
                }	
         
        }
	if(array[0]==1){
        tosses_within_circle+=circle_in;
        number_of_tosses_so_far+=circle_out;
	}
	pthread_exit(NULL);

}





