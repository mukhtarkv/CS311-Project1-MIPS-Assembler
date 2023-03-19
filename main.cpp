#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "assembler.h"
using namespace std;


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

		ifstream infile(file);

		// if(freopen(file, "r",stdin)==0){
		// 	printf("File open Error!\n");
		// 	exit(1);
		// }

		//From now on, if you want to read string from input file, you can just use scanf function.
		// Create an empty vector
    	vector<string> assembly_lines;
		string line;
		while (getline(infile, line))
		{
			assembly_lines.push_back(line);
		}

		// For output file write 
		// You can see your code's output in the sample_input/example#.o 
		// So you can check what is the difference between your output and the answer directly if you see that file
		// make test command will compare your output with the answer
		file[strlen(file)-1] ='o';
		freopen(file,"w",stdout);

		//If you use printf from now on, the result will be written to the output file.
		vector<string> output_lines = assemble(assembly_lines);
		for (string output_line : output_lines) {
			cout << output_line;
		}
	}
	return 0;
}

