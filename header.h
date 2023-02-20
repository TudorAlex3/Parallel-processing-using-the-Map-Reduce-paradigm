#include <iostream>
#include <pthread.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string.h>
#include <queue>
#include <math.h>

using namespace std;

// Structura pentru thread-uri Map
typedef struct map_thread {
    int id;
    int reduce;
    queue<string> *files;
    vector<vector<int>> *results;
    pthread_mutex_t *read_mutex;
    pthread_mutex_t *write_mutex;
    pthread_barrier_t *barrier;
} map_thread;

// Structura pentru thread-uri Reduce
typedef struct reduce_thread {
    int id;
    int reduce;
    vector<vector<int>> *results;
    pthread_barrier_t *barrier;
} reduce_thread;

// Funcții utilizate
bool sortFilesBySize(string file1, string file2);
bool binarySearch(int left, int right, int number, int power);
void *f_map(void *arg);
void *f_reduce(void *arg);
