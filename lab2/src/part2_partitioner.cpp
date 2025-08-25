#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>

using namespace std;

int main(int argc, char **argv)
{
	if (argc != 6)
	{
		cout << "usage: ./partitioner.out <path-to-file> <pattern> <search-start-position> <search-end-position> <max-chunk-size>\nprovided arguments:\n";
		for (int i = 0; i < argc; i++)
			cout << argv[i] << "\n";
		return -1;
	}

	char *file_to_search_in = argv[1];
	char *pattern_to_search_for = argv[2];
	int search_start_position = atoi(argv[3]);
	int search_end_position = atoi(argv[4]);
	int max_chunk_size = atoi(argv[5]);

	cout << "[" << getpid() << "] start position = " << search_start_position << " ; end position = " << search_end_position << "\n";
	if ((search_end_position - search_start_position) > max_chunk_size)
	{
		int divide = search_start_position + (search_end_position - search_start_position) / 2;
		string _start = to_string(search_start_position);
		string _mid = to_string(divide);

		string _sec_start = to_string(divide + 1);
		string _end = to_string(search_end_position);
		string _chunk_size = to_string(max_chunk_size);

		char *LEFTARGS[7], *RIGHTARGS[7];

		for (int i = 0; i < 6; i++)
		{
			LEFTARGS[i] = (char *)calloc(1, 100);
			RIGHTARGS[i] = (char *)calloc(1, 100);
		}
		LEFTARGS[6] = RIGHTARGS[6] = NULL;
		strcpy(LEFTARGS[0], argv[0]);
		strcpy(LEFTARGS[1], file_to_search_in);
		strcpy(LEFTARGS[2], pattern_to_search_for);
		strcpy(LEFTARGS[3], _start.c_str());
		strcpy(LEFTARGS[4], _mid.c_str());
		strcpy(LEFTARGS[5], argv[5]);

		strcpy(RIGHTARGS[0], argv[0]);
		strcpy(RIGHTARGS[1], file_to_search_in);
		strcpy(RIGHTARGS[2], pattern_to_search_for);

		strcpy(RIGHTARGS[3], _sec_start.c_str());
		strcpy(RIGHTARGS[4], _end.c_str());
		strcpy(RIGHTARGS[5], argv[5]);

		pid_t left_child = fork();
		
		if (left_child == -1)
		{
			perror("fork LEFT failed");
			exit(EXIT_FAILURE);
		}
		else if (left_child == 0)
		{
			if (execvp(LEFTARGS[0], LEFTARGS) == -1)
			{
				perror("execvp LEFT failed");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			cout << "[" << getppid() << "] forked left child " << getpid() << "\n";
			wait(NULL);
			cout << "[" << getppid() << "] left child returned\n";
		}
		pid_t right_child = fork();
		if (right_child == -1)
		{
			perror("fork Right failed");
			exit(EXIT_FAILURE);
		}
		else if (right_child == 0)
		{
			if (execvp(RIGHTARGS[0], RIGHTARGS) == -1)
			{
				perror("execvp RIGHT failed");
			}
		}
		else
		{
			cout << "[" << getppid() << "] forked right child " << getpid() << "\n";
			wait(NULL);
			cout << "[" << getppid() << "] right child returned\n";
		}
	}
	else
	{
		cout << "[" << getppid() << "] forked searcher child " << getpid() << "\n";
		int length_of_pattern = strlen(pattern_to_search_for);

		ifstream file(file_to_search_in);

		if (!file.is_open())
		{
			cerr << "Error opening file!" << endl;
		}
		file.seekg(search_start_position, ios::beg);
		streamsize buffer = 0;
		char *string;
		
		if (max_chunk_size > 1024) {
			buffer = 1025;
		}
		else {
			buffer = (streamsize)max_chunk_size;
		}
		string = (char *)calloc(1, buffer+1);
		string[buffer] = '\0';
		char *result;
		int found = 0;

		int loop = 0;
		while (loop < search_end_position && file.read(string, buffer))
		{
			int bytesRead = file.gcount();

			string[bytesRead] = '\0';
			result = strstr(string, pattern_to_search_for);
			if (result != NULL)
			{
				found = 1;

				cout << "[" << getpid() << "] Substring found at position: " << (search_start_position + loop + result - string) << endl;
			}
			loop += bytesRead;
		}
		if (!found)
		{
			cout << "[" << getpid() << "] didn't find\n";
		}
		free(string);
		file.close();
	}

	return 0;
}

// TODO
// cout << "[" << my_pid << "] start position = " << search_start_position << " ; end position = " << search_end_position << "\n";
// cout << "[" << my_pid << "] left child returned\n";
// cout << "[" << my_pid << "] right child returned\n";*/
// cout << "[" << my_pid << "] searcher child returned \n";
// cout << "[" << my_pid << "] received SIGTERM\n"; //applicable for Part III of the assignment
