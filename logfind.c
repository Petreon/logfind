#include "dbg.h"
#include <stdlib.h>
#include <string.h>
#include "logfind.h"
#include <stdio.h>
#include <io.h>

// in this exercise we need to create a grep command
// so i have a .logfind archive that have the archives that i want to find these words
/*
* things to do:
* - get the input from argc and argv, handle with -o (that is a option for OR logical between the words)
*  if not o, so all the words need to be in the file
* - get all the files from the directory that you using logfile and save in a list
* - use fgets to read all the archive and all the archives from the list and save the archive that the word appears
* - print all the files that word appears
* - end program
*/

/*
* REASONS TO I NOT USING THE PATH_LOCAL:
* it's get the actual path where the executable is, but the dir command get the local where the user is calling the exec
* that is the behavior that we want
*/

#define MAX_DATA 128

void destroy_data(Logfind* app) {

	if (app) {
		if (app->word_array) {
			for (int i = 0; app->word_array[i] != NULL; i++) {
				if(app->word_array[i])
					free(app->word_array[i]);
			}
		}
		if (app->files_to_search_array) {
			for (int i = 0; app->files_to_search_array[i] != NULL; i++) {
				if(app->files_to_search_array[i])
					free(app->files_to_search_array[i]);
			}
		}
		if (app->path_local)
			free(app->path_local);
		free(app);
	}

}

// dont need this function anymore but its good in case, and only works for windows i think
char* get_path(char* path) {
	//getting the path file
	int bar_count = 0;
	char* new_local = malloc((strlen(path) * sizeof(char)) + 1);
	memset(new_local, 0, (strlen(path) * sizeof(char)) + 1);
	int i = 0;

	//handling the actual local
	for (i = strlen(path); i >= 0; i--) {
		if (path[i] == '\\' && bar_count == 0) {
			bar_count++;
		}
		else if (bar_count > 0) {
			new_local[i] = path[i];
		}
	}
	strcat(new_local, "\\");
	return new_local;

}

char get_logical(char* logical) {
	char logical_char;

	if (logical[0] == '-') {
		if (strlen(logical) != 2)
			log_err("Logical size wrong.");
		//check(logical[1] == 'o', "Invalid logical argument");
		return 'o';
	}

	return 'a';

error:
	return 'z';
}

// getting the word array
void get_word_array(Logfind* app, int size_array, char* word_array[]) {
	//do an string copy for the word_array pointer to return to the app

	char word_buffer[MAX_DATA];
	int initial_index;
	int real_size;
	//checking for get the right size of inputs string array
	if (app->logical == 'o') {
		initial_index = 2;
		real_size = size_array - 2;
	}
	else {
		initial_index = 1;
		real_size = size_array - 1;
	}

	//initializing the size of word_array
	//free(app->word_array);
	app->word_array = malloc((real_size + 1) * sizeof(char*));
	memset(app->word_array, 0, (real_size+1) * sizeof(char*));
	if (app->word_array == NULL) {
		log_err("The array is null %p", app->word_array);
	}

	

	for (int i = initial_index; i < size_array; i++) {
		strcpy(word_buffer, word_array[i]);
		app->word_array[i - initial_index] = malloc((MAX_DATA+1) * sizeof(char));
		memset(app->word_array[i - initial_index], 0, (MAX_DATA + 1) * sizeof(char));
		memmove(app->word_array[i - initial_index], word_buffer, ((MAX_DATA + 1) * sizeof(char)));
		
	}

	return;

error:
	// free the app

	return -1;
}


//first get the input from the user, so in the constructor from log find we will get all the words and save them
Logfind* start_app(int argc, char* argv[]) {

	Logfind* new_app = malloc(sizeof(Logfind));
	memset(new_app, 0, sizeof(Logfind));

	//getting the local to get the archives
	char* str_buffer = get_path(argv[0]);
	new_app->path_local = malloc((strlen(str_buffer) * sizeof(char))+1);
	strncpy(new_app->path_local, str_buffer, (strlen(str_buffer) * sizeof(char))+1);
	free(str_buffer);
	new_app->logical = get_logical(argv[1]);
	check(new_app->logical != 'z', "Error initializing the logical");
	get_word_array(new_app, argc, argv);
	

	return new_app;

error:
	return NULL;
}

int file_count_dir() {
	// counting the same pipe of the get_directory_files_function
	FILE* pipe = _popen("dir /b", "r");
	int count = 0;
	char buffer = ' ';
	while (pipe != EOF) {
		buffer = fgetc(pipe);
		if (buffer == '\n') {
			count++;
		}
		if (buffer == EOF) break;
	}
	fclose(pipe);
	return count;
}

int get_directory_files(Logfind* app) {
	// usign the pipe approach from gemini

	//1° creating a pipe
	/*
	* the _popen function redirects the stdout for another place without change the original stdout
	*/
	FILE* pipe_fp;
	char* str_buffer = malloc(MAX_DATA * sizeof(char)); // only accepts archives with less than 128 len
	memset(str_buffer, 0, MAX_DATA * sizeof(char));
	char buffer = ' ';
	pipe_fp = _popen("dir /b", "r");
	int i = 0;
	int file_count = 0;
	int j = 0;

	// first i will count the files to help in the memory allocate this is reduce speed, but get less memory errors
	file_count = file_count_dir();

	char** str_array = malloc((file_count+1) * sizeof(char*)); // initializing the string array with the correct size
	memset(str_array, 0, (file_count + 1) * sizeof(char*)); // setting everything to null to handle better in the free

	//i cannot do a rewind in the pipe stream so inside the file_count function i called another pipe
	//rewind(pipe_fp);

	while (i <= file_count) {
		buffer = fgetc(pipe_fp);
		if (buffer != '\n' && buffer != EOF) {
			str_buffer[j++] = buffer;
		}
		else if(buffer != EOF) {
			//looping throw the array to append the files
			j = 0;
			str_array[i] = malloc((strlen(str_buffer)+1) * sizeof(char));
			strcpy(str_array[i], str_buffer);
			memset(str_buffer, 0, MAX_DATA * sizeof(char));
			i++;
		}
		else {
			break;
		}
	}
	free(str_buffer);
	app->files_to_search_array = str_array;

	//system("dir /b"); this dont need anymore becaus _popen do this for us
	//log_info("Reached");

	return 0;

error:
	return -1;
}


int word_in_file(Logfind* app, char *str_file, char *str_word) {
	char* buffer = malloc((strlen(str_word) + 1) * sizeof(char));
	memset(buffer, 0, (strlen(str_word) + 1) * sizeof(char));
	int bool = 0;

	for (int i = 0; i < strlen(str_file); i++) {

		for (int j = i; j < strlen(str_word)+i; j++) {
			if (str_file[j] != NULL) {
				buffer[j - i] = str_file[j];
			}
		}

		bool = strcmp(buffer, str_word);
		if (bool == 0) {
			return 1;
		}
		memset(buffer, 0, (strlen(str_word) + 1) * sizeof(char));
	}

	return 0;

}

//WHEN I SEE THS CODE IN THE NEXT MONTH I CANT UNDERSTAND ANYMORE
void find_word_in_files(Logfind* app) {
	// THIS IS THE WORST CODE OF MY LIFE BUT I WANT TO MOVE FORWARD
	FILE* actual_file;
	char* buffer_str_file;
	char buffer;

	int already_printed = 0;

	for (int i = 0; app->files_to_search_array[i] != NULL; i++) {
		
		actual_file = fopen(app->files_to_search_array[i], "r");
		
		//this prevents for trying to open a folder
		if (actual_file != NULL) {
			//getting the file size to initilizae the buffer_str_file correctly
			fseek(actual_file, 0L, SEEK_END);
			int size_file = ftell(actual_file);

			int k = 0;
			rewind(actual_file);

			buffer_str_file = malloc((size_file + 1) * sizeof(char));
			memset(buffer_str_file, 0, (size_file + 1) * sizeof(char));
			for (int j = 0; app->word_array[j] != NULL; j++) {

				//reading all the file in one time
				while ((buffer = fgetc(actual_file)) != EOF) {
					buffer_str_file[k] = buffer;
					k++;
				}

				//return a value for know if print or not the file this is for 'and' logic
				if (app->logical == 'a') {
					// this logic is poor, but is good for improvements in systems like, what word is in the archive
						// in this case i will do an array of logics, if all are 1 for every word i can print the archive
					int bool = 0;
					for (int z = 0; app->word_array[z] != NULL; z++) {
						bool = word_in_file(app, buffer_str_file, app->word_array[z]);
						if (bool == 0) {
							break;
						}
					}

					if (bool) {
						if (!already_printed)
							printf("%s\n", app->files_to_search_array[i]);
						already_printed++;
					}
				}
					// -o "or" logic
				else {
					
					if (word_in_file(app, buffer_str_file, app->word_array[j])) {
						// its better to do a function for this
						if (!already_printed)
							printf("%s\n", app->files_to_search_array[i]);
						already_printed++;
					}

				}

			}
			free(buffer_str_file);
			already_printed = 0;
		}

	}

}

//paulo

int main(int argc, char* argv[]) {
	Logfind* app = NULL;
	check(argc > 1, "USAGE: <and(default) or or(-o)> <word> ");

	app = start_app(argc, argv);
	check(app != NULL, "Application didn't start well");

	//starting get the files to search

	int rc = get_directory_files(app);
	check(rc != -1, "Error opening the file");

	// now run in the files to find the words
	find_word_in_files(app);

	destroy_data(app);
	return 0;

error:
	//future create a function to free the variables
	destroy_data(app);
	return -1;
}