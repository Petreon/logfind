#ifndef LOGFIND_H
#define LOGFIND_H
#endif

typedef struct Logfind {

	char **word_array;
	char logical;
	char* path_local;
	char **files_to_search_array;

} Logfind;


char* get_path(char* path);

char get_logical(char* arg1);

void get_word_array(Logfind* app, int size_array, char* word_array[]);

int get_directory_files(Logfind* app);

Logfind* start_app(int argc, char* argv[]);

int word_in_file(Logfind* app, char* str_file, char* str_word);

void find_word_in_files(Logfind* app);