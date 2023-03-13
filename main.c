#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char* argv[]){

	if(argc != 2){
		printf("Usage: ./runfile <assembly file>\n"); //Example) ./runfile /sample_input/example1.s
		printf("Example) ./runfile ./sample_input/example1.s\n");
		exit(0);
	}
	else
	{

		// To help you handle the file IO, the deafult code is provided.
		// If we use freopen, we don't need to use fscanf, fprint,..etc. 
		// You can just use scanf or printf function 
		// ** You don't need to modify this part **
		// If you are not famailiar with freopen,  you can see the following reference
		// http://www.cplusplus.com/reference/cstdio/freopen/

		//For input file read (sample_input/example*.s)

		char *file=(char *)malloc(strlen(argv[1])+3);
		strncpy(file,argv[1],strlen(argv[1]));

		if(freopen(file, "r",stdin)==0){
			printf("File open Error!\n");
			exit(1);
		}

		//From now on, if you want to read string from input file, you can just use scanf function.


		// For output file write 
		// You can see your code's output in the sample_input/example#.o 
		// So you can check what is the difference between your output and the answer directly if you see that file
		// make test command will compare your output with the answer
		file[strlen(file)-1] ='o';
		freopen(file,"w",stdout);

		//If you use printf from now on, the result will be written to the output file.

		printf("Hello World!\n"); 
		

	}
	return 0;
}

