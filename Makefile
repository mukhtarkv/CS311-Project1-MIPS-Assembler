CC=gcc

default: runfile

runfile.o: main.c 
	$(CC) -c main.c -std=c99 -o runfile.o
 
runfile: runfile.o
	$(CC) runfile.o  -o runfile

test: default test_1 test_2 test_3 test_4 test_5 

test_1:
	@echo "Testing example01"; \
		./runfile sample_input/example1.s &&  diff -Naur sample_input/example1.o sample_output/example1.o ;\
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi \

test_2:
	@echo "Testing example02"; \
		./runfile sample_input/example2_mod.s && diff -Naur sample_input/example2_mod.o sample_output/example2_mod.o ;\
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_3:
	@echo "Testing example03"; \
		./runfile sample_input/example3.s && diff -Naur sample_input/example3.o sample_output/example3.o  ;\
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_4:
	@echo "Testing example04"; \
		./runfile sample_input/example4.s &&  diff -Naur sample_input/example4.o sample_output/example4.o  ;\
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi

test_5:
	@echo "Testing example05"; \
		./runfile sample_input/example5.s &&  diff -Naur sample_input/example5.o sample_output/example5.o ;\
		if [ $$? -eq 0 ]; then echo "\tTest seems correct\n"; else echo "\tResults not identical, check the diff output\n"; fi


clean:
	rm -f *.o
	rm -f runfile


