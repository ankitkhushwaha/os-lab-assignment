build-part1: part1.out
build-part2: part2.out

part1.out: part1_searcher.cpp
	g++ -g part1_searcher.cpp -o part1.out

part2.out: part2_partitioner.cpp
	g++ -g part2_partitioner.cpp -o part2.out

run-part1: part1.out
	./part1.out file.txt NGTNIJGK 0 67108863

run-part2: part2.out
	./part2.out $(FILE) $(PATTERN) $(START) $(END) $(BATCHSIZE)

gdb-part1: part1.out
	gdb --args ./part1.out file.txt NGTNIJGK 0 67108863	

gdb-part2: part2.out
	gdb --args ./part2.out file.txt NGTNIJGK 0 128 32

clean-part1:
	rm part1.out


clean:
	rm *.out
