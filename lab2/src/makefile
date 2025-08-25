build-part1: part1.out
build-part2: part2.out
build-part3: part3.out


part1.out: part1_searcher.cpp
	g++ -g part1_searcher.cpp -o part1.out

run-part1: part1.out
	./part1.out file.txt NGTNIJGK 0 67108863

part2.out: part2_partitioner.cpp part2_searcher.cpp
	g++ -g part2_partitioner.cpp -o part2_partitioner.out
# 	g++ -g part2_searcher.cpp -o part2_searcher.out

run-part2: part2.out
# 	./part2_searcher.out file.txt NGTNIJGK 0 67108863
	./part2_partitioner.out file.txt NGTNIJGK 0 67108863 8388608

part3.out: part3_searcher.cpp
	g++ -g part3_searcher.cpp -o part3_searcher.out
	g++ -g part3_partitioner.cpp -o part3_partitioner.out

run-part3: part3.out
# 	./part3_searcher.out file.txt NGTNIJGK 0 67108863
	./part3_partitioner.out file.txt NGTNIJGK 0 67108863 8388608


# https://stackoverflow.com/questions/24614856/how-to-run-multiple-command-in-a-single-line

gdb-part1: part1.out
	gdb --args ./part1.out file.txt NGTNIJGK 0 67108863	

gdb-part2: part2.out
	gdb --args ./part2.out file.txt NGTNIJGK 0 67108863 8388608

gdb-part3: part3_searcher.cpp
	gdb --args ./part3_searcher.out file.txt NGTNIJGK 0 67108863	


clean-part1:
	rm part1.out


clean:
	rm *.out
