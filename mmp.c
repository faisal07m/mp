
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
MPI_Win window;


/*
	A block struct contains one "task" to do.
	It represents a square area of the fractal to be rendered with the upper left point at x,y and lengths of size.
	it also contains information as to where to insert the calculated pixels as a starting shift (target_pos) and a size in pixels (target_size)
*/
typedef struct Block_{
	//Input
	float x;//Upper left point
	float y;//Upper left point
	float size;
	//Output pixels
	int target_pos;
	int target_size;//Assumes a square!
} Block;



int checkMandelbrot(float real, float imag, int cutoff){
	 /*
        Task 2
		------
		Implement the actual logic for determining whether a point is in the mandelbrot set (https://en.wikipedia.org/wiki/Mandelbrot_set).
		f_(n+1)(z) = f_n(z) + z,
		 z = real + i*imag from the argument.
		Perform the iteration up to as many times as the cutoff states, at which point you assume the point is in the set.
		If the absolute value is ever greater than 2, the series will diverge for sure and you can conclude that the point is not in the set

		Notes:
			- You can do this test without calling the expensive square root function.

		Solution:
			- Looks like the set in wikipedia.
	*/

    float r = real, re_c = real, im = imag, im_c = imag;
    int cutoff_counter = 0;
    while( cutoff_counter < cutoff ) {
      
       if ((r * r + im * im) <=4 ){
        r = real * real - im * im + re_c;
        im = 2 * real * im + im_c;

        real = r;
        cutoff_counter++;
        
        }

        else{
  
          return 0;
         }

        }
    
	//if(real > imag) return cutoff;
	return cutoff;
}



void HandleBlock(int my_rank, Block block, int total_size_x, int**my_local_results, int max_number_iterations){

	for(int v=0;v<block.target_size;++v){
			for(int b=0;b<block.target_size;++b){

				int result = checkMandelbrot(
					block.x + block.size * b / block.target_size
					,block.y + block.size * v / block.target_size
					, max_number_iterations
				);

				(*my_local_results)[v*block.target_size+b] = result; //my_rank * 100 + b + v;
			}
	}


	for(int v=0;v<block.target_size;++v){

		 /*
			Task 1
			------
			Transfer the pixels calculated above to the memory exposed by Process 0 using MPI_Put

			Notes:
				- Due to the different layouts, you need to do this line by line
				- Target location for each line is the base position(block.target_pos) + an offset(v*total_size_x) that depends on the dimensions of the target matrix
				- MPI2: Lock and unlock, even if nobody else will write to the same spot. This causes the wirte to happen before we overwrite our local results with the next block.
				- MPI3: You can also use MPI_Rput and get an MPI_request from that. Then wait for the request, which is fulfilled as soon as the buffer is ready for reuse.
			Solution:
				- A triangular shape appears as the output
		*/

        MPI_Win_lock( MPI_LOCK_EXCLUSIVE, 0, 0, window);

        MPI_Put( (*my_local_results) + v*block.target_size, block.target_size, MPI_INT, 0, (v * total_size_x) + block.target_pos, block.target_size, MPI_INT, window);

        MPI_Win_unlock(0, window);

	}
}


int main(int argc, char *argv[]){
	/*
		We render a square area with side lengths size (3rd command line parameter) with its upper left point given by the first two parameters.
		A fourth parameter sets the maximum number of iterations, to possibly bring out finer details.
	*/
	int output_size_pixels = 2000;
	float pos_x = -1.80f;
	float pos_y = -2.0f;
	float size = 5.0f;
	int max_number_iterations = 100;

	//MPI seems to do fine with providing these to all participating processes
	if(argc >= 4){
		pos_x = atof(argv[1]);
		pos_y = atof(argv[2]);
		size = atof(argv[3]);
	}
	if(argc >= 5){
		max_number_iterations = atoi(argv[4]);
	}


 	static unsigned char white[3];
	white[0]=255; white[1]=255; white[2]=255;
 	static unsigned char black[3];
	black[0]=0; black[1]=0; black[2]=0;
 	static unsigned char greyscale[3];
	greyscale[0]=0; greyscale[1]=0; greyscale[2]=0;



	MPI_Init(&argc,&argv);
	int my_rank,total_ranks;
	MPI_Comm_size(MPI_COMM_WORLD,&total_ranks);
	MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);

	int *shared_data;

	/*
			Task 3
			------
			Create and distribute multiple Blocks in such a way, that more than one process can work at a time and the likelyhood of load imbalances is low

			Notes:
				- Each of the n processes should work on n blocks, which ideally are not oriented as a row or column to reduce the likelyhood of load imbalances

			Solution:
				- Should yield a speedup over the previous version, scaling with the number of processes used.
				- Try higher numbers of iterations.
	*/


	if(my_rank == 0){
		//Allocate memory and make available to others

		MPI_Aint siz = output_size_pixels * output_size_pixels * sizeof(int);
		MPI_Alloc_mem(siz, MPI_INFO_NULL, &shared_data);
		MPI_Win_create(shared_data, siz, sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD, &window);

	} else {
		//All other ranks make no memory available for remote access
		MPI_Win_create(shared_data, 0,sizeof(int),MPI_INFO_NULL, MPI_COMM_WORLD, &window);
	}

	//See below
	int *my_result_vals;

	int blocks_per_direction = total_ranks;

    // 1 block per direction
	int total_blocks = blocks_per_direction * blocks_per_direction;

    // 1 block of pixel size 2000
	int block_pixel_size = output_size_pixels / blocks_per_direction;

	//Allocate a block of memory to keep our local results until sending
	my_result_vals =(int *)malloc(sizeof(int)*(block_pixel_size  * block_pixel_size));

    int block_coord_x = 0; //In full blocks
	int block_coord_y = 0;

    for( int i = 0; i < total_ranks; i++) {  
		Block block;

        if( i % 2 == 0) {
            block_coord_x = my_rank;

        } else {
            block_coord_x = total_ranks - my_rank - 1;

        }
        block_coord_y = i;
        
		block.x = pos_x + size / blocks_per_direction * block_coord_x;//upper left corner of the block we want to manage
		block.y = pos_y + size / blocks_per_direction * block_coord_y;
		block.size = size / blocks_per_direction;

		block.target_size = output_size_pixels / blocks_per_direction;
		//Start of the first row in our target matrix.
		//Second row will start with an offset of +output_size_pixels, etc.
		block.target_pos = block.target_size * (block_coord_x + block_coord_y * output_size_pixels);

		HandleBlock(my_rank,block,output_size_pixels, &my_result_vals, max_number_iterations);
    }


	//Before outputting the result we wait for all the values
	MPI_Barrier(MPI_COMM_WORLD);

	//Temporary storage no longer needed
	free(my_result_vals);


	if(my_rank == 0){
		char filename[100];
		sprintf(filename,"Mandelbrot_x%f y%f size %f.ppm",pos_x,pos_y,size);
		FILE *fp = fopen(filename, "wb"); /* b - binary mode */
		fprintf(fp, "P6\n%d %d\n255\n", output_size_pixels,output_size_pixels);



		for(int i=0;i<output_size_pixels;++i){
			for(int b=0;b<output_size_pixels;++b){
				int res = shared_data[i*output_size_pixels + b];
				if( res > 0){
					greyscale[0]=250-200*res/max_number_iterations;
					greyscale[1]=250-200*res/max_number_iterations;
					greyscale[2]=250-200*res/max_number_iterations;
					fwrite(greyscale, 1 , 3 , fp);
				} else {
					fwrite(black, 1 , 3 , fp);
				}

			}
		}

		fclose(fp);
	}

	MPI_Win_free(&window);
	MPI_Finalize();

	return 0;
}
