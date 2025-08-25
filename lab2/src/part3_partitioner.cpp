#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstring>

using namespace std;
char *CHILD_TYPE[2] = {
	"LEFT",
	"RIGHT",
};

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

	int exit_status;
	cout << "[" << getpid() << "] start position = " << search_start_position << " ; end position = " << search_end_position << "\n";
	if ((search_end_position - search_start_position) > max_chunk_size)
	{
		pid_t my_children[2];
		int divide = search_start_position + (search_end_position - search_start_position) / 2;
		string _start = to_string(search_start_position);
		string _mid = to_string(divide);

		string _sec_start = to_string(divide + 1);
		string _end = to_string(search_end_position);
		string _chunk_size = to_string(max_chunk_size);

		char *ARGS[2][7] = {
			{argv[0], file_to_search_in, pattern_to_search_for, (char *)_start.c_str(), (char *)_mid.c_str(), argv[5], NULL},
			{argv[0], file_to_search_in, pattern_to_search_for, (char *)_start.c_str(), (char *)_mid.c_str(), argv[5], NULL}};

		for (int i = 0; i < 2; i++)
		{
			pid_t cpid = fork();
			if (cpid == -1)
			{
				fprintf(stderr, "fork %s failed\n", CHILD_TYPE[i]);
				exit(EXIT_FAILURE);
			}
			else if (cpid == 0)
			{
				if (execvp(ARGS[i][0], ARGS[i]) == -1)
				{
					fprintf(stderr, "execvp %s failed\n", CHILD_TYPE[i]);
					exit(EXIT_FAILURE);
				}
			}
			else
			{
				fprintf(stdout, "[%ld]forked %s child %ld\n", getpid, CHILD_TYPE[i], cpid);
				my_children[i] = cpid;
			}
		}
		int found = 0;
	}

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

	if (max_chunk_size > 1024)
	{
		buffer = 1025;
	}
	else
	{
		buffer = (streamsize)max_chunk_size;
	}
	string = (char *)calloc(1, buffer + 1);
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
			return 0;
		}
		loop += bytesRead;
	}
	if (!found)
	{
		cout << "[" << getpid() << "] didn't find\n";
	}

	return 0;
}

// TODO
// cout << "[" << my_pid << "] start position = " << search_start_position << " ; end position = " << search_end_position << "\n";
// cout << "[" << my_pid << "] left child returned\n";
// cout << "[" << my_pid << "] right child returned\n";*/
// cout << "[" << my_pid << "] searcher child returned \n";

// else
// {
// 	int left_status;
// 	int left = waitpid(left_child, &left_status, 0);

// 	if (left == -1)
// 	{
// 		perror("waitpid left failed");
// 		exit(EXIT_FAILURE);
// 	}
// 	else if (WIFEXITED(left_status))
// 	{
// 		exit_status = WEXITSTATUS(left_status);
// 	}
// 	cout << "[" << left_child << "] left child returned\n";
// }

// pid_t right_child = fork();
// if (right_child == -1)
// {
// 	perror("fork Right failed");
// 	exit(EXIT_FAILURE);
// }
// else if (right_child == 0)
// {
// 	if (execvp(RIGHTARGS[0], RIGHTARGS) == -1)
// 	{
// 		perror("execvp RIGHT failed");
// 	}
// }
// else
// {
// 	cout << "[" << getpid() << "] forked right child " << right_child << "\n";
// 	int right_status;
// 	int right = waitpid(left_child, &right_status, 0);

// 	if (right == -1)
// 	{
// 		perror("waitpid right failed");
// 		exit(EXIT_FAILURE);
// 	}
// 	if (exit_status == 3)
// 	{
// 		kill(right_child, SIGTERM);
// 		cout << "[" << getpid() << "] received SIGTERM\n"; // applicable for Part III of the assignment
// 	}
// 	cout << "[" << getpid() << "] right child returned\n";
// }
