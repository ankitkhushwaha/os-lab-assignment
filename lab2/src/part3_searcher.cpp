#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <signal.h>

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 5)
	{
		cout << "usage: ./partitioner.out <path-to-file> <pattern> <search-start-position> <search-end-position>\nprovided arguments:\n";
		for (int i = 0; i < argc; i++)
			cout << argv[i] << "\n";
		return -1;
	}

	char *file_to_search_in = argv[1];
	char *pattern_to_search_for = argv[2];
	int search_start_position = atoi(argv[3]);
	int search_end_position = atoi(argv[4]);

	// TODO
	int length_of_pattern = strlen(pattern_to_search_for);

	ifstream file(file_to_search_in);

	if (!file.is_open())
	{
		cerr << "Error opening file!" << endl;
	}

	streamsize buffer = 1024;
	char string[1025];
	char *result;
	int found = 0;
	int loop = 0;

	while (!file.eof())
	{
		file.read(string, buffer);
		int bytesRead = file.gcount();
		string[bytesRead] = '\0';
		result = strstr(string, pattern_to_search_for);
		if (result != NULL)
		{
			found = 1;

			cout << "[" << getppid() << "] Substring found at position: " << (search_start_position + loop + result - string) << endl;

			pid_t parent = getppid();
			kill(parent, 15);
		}
		loop += bytesRead;
	}
	if (!found)
	{
		cout << "[-1] didn't find\n";
	}
	file.close();

	return 0;
}
