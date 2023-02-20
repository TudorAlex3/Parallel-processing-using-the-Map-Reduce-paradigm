#include "header.h"

// Sortează fișierele de intrare descrescător, după dimensiune
bool sortFilesBySize(string file1, string file2) {

    const char *file_a = file1.c_str();
    const char *file_b = file2.c_str();

    FILE *f1 = fopen(file_a, "r");
    FILE *f2 = fopen(file_b, "r");

    fseek(f1, 0, SEEK_END);
    fseek(f2, 0, SEEK_END);
    int size1 = ftell(f1);
    int size2 = ftell(f2);

    fclose(f1);
    fclose(f2);

    return size1 > size2;
}

// Căutare binară pentru găsirea unui număr care ridicat la o putere dă
// un rezultat egal cu numărul din fișierul de intrare
bool binarySearch(int left, int right, int number, int power) {
    if (right >= left) {
        int mid = left + (right - left) / 2;

        if (pow(mid, power) == number)
            return true;

        if (pow(mid, power) > number)
            return binarySearch(left, mid - 1, number, power);
 
        return binarySearch(mid + 1, right, number, power);
    }

    return false;
}

// Funcția Map
void *f_map(void *arg) {
	map_thread thread = *(map_thread *)arg;

    // Cât timp mai sunt fișiere în coadă
    while (!thread.files->empty()) {
        string file_path;

        // Thread-ul își ia un fișier și îl elimină din coadă
        pthread_mutex_lock(thread.read_mutex);
        file_path = thread.files->front();
        thread.files->pop();
        pthread_mutex_unlock(thread.read_mutex);

        ifstream input;
        input.open(file_path.c_str());

        int number_of_numbers;
        vector<int> numbers;
        string number;

        // Parcurge fișierul linie cu linie și memorează numerele în vector
        input >> number_of_numbers;
        for (int i = 0; i < number_of_numbers; i++) {
            input >> number;
            if (number.back() == '\n')
                number.erase(number.back());
            if(stoi(number) > 0)
                numbers.push_back(stoi(number));
        }
        input.close();

        // results conține câte o listă pentru fiecare putere
        vector<vector<int>> results(thread.reduce);

        for (int i = 0; i < (int)numbers.size(); i++) {
            for (int j = 0; j < thread.reduce; j++) {
                // 1 face parte din orice listă
                if (numbers[i] == 1)
                    results.at(j).push_back(1);
                else if (binarySearch(2, sqrt(numbers[i]), numbers[i], j + 2))
                    results.at(j).push_back(numbers[i]);
            }
        }

        // Fiecare listă rezultată se pune în lista de rezultate
        pthread_mutex_lock(thread.write_mutex);
        for (int i = 0; i < (int)results.size(); i++)
            thread.results->push_back(results.at(i));
        pthread_mutex_unlock(thread.write_mutex);
    }

    pthread_barrier_wait(thread.barrier);
	pthread_exit(NULL);
}

// Funcția Reduce
void *f_reduce(void *arg) {   
	reduce_thread thread = *(reduce_thread *)arg;
    pthread_barrier_wait(thread.barrier);

    // Se parcurg rezultatele funcției Map și pentru fiecare listă cu indicele
    // specific ID-ului thread-ului Reduce, se adaugă elementele într-un nou
    // vector ce reprezintă rezultatul final al reducer-ului
    vector<int> concat_list;
    for (int i = 0; i < (int)thread.results->size(); i++) {
        if (i % thread.reduce == thread.id)
         for (int j = 0; j < (int)thread.results->at(i).size(); j++) {
            // Verific dacă acest element face parte deja din listă
            int element = thread.results->at(i).at(j);
            vector<int>::iterator it;
            it = find(concat_list.begin(), concat_list.end(), element);
            
            if (it == concat_list.end())
                concat_list.push_back(thread.results->at(i).at(j));
        }
    }

    // Se creează fișierul de ieșire corespunzător puterii
    string output = "out";
    output.append(to_string(thread.id + 2));
    output.append(".txt");

    ofstream output_file;
    output_file.open(output.c_str());
    output_file << (int)concat_list.size();
    output_file.close();

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    int map_number = stoi(argv[1]);
    int reduce_number = stoi(argv[2]);

    ifstream file;
    file.open(argv[3]);

    // Adaug numele fișierelor de intrare într-un vector
    vector<string> files;
    string line;
    int files_number;

    file >> files_number;
	for (int i = 0; i < files_number; i++) {
        file >> line;
        if (line.back() == '\n')
            line.erase(line.back());
        files.push_back(line);
    }
    file.close();

    // Sortez vectorul descrescător, după dimensiune
    sort(files.begin(), files.end(), sortFilesBySize);

    queue<string> sorted_files;
    for (int i = 0; i < files_number; i++)
        sorted_files.push(files[i]);

    // Variabile folosite pentru crearea thread-urilor
    void *status;
    pthread_t map_threads[map_number];
    pthread_t reduce_threads[reduce_number];
    pthread_mutex_t read_mutex;
    pthread_mutex_t write_mutex;
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, map_number + reduce_number);
	pthread_mutex_init(&read_mutex, NULL);
    pthread_mutex_init(&write_mutex, NULL);

    // map_results reprezintă lista ce conține toate listele rezultate în urma
    // acțiunii fiecărui thread map.
    vector<vector<int>> *map_results = new vector<vector<int>>();
    map_thread map_arguments[map_number];
    reduce_thread reduce_arguments[reduce_number];

    int k = 0;
	for (int i = 0; i < map_number + reduce_number; i++) {
        if (i < map_number) {
            map_arguments[i].id = i;
            map_arguments[i].reduce = reduce_number;
            map_arguments[i].files = &sorted_files;
            map_arguments[i].results = map_results;
            map_arguments[i].read_mutex = &read_mutex;
            map_arguments[i].write_mutex = &write_mutex;
            map_arguments[i].barrier = &barrier;

		    int r = pthread_create(&map_threads[i], NULL,
                                    f_map, &map_arguments[i]);

            if (r) {
                printf("Eroare la crearea thread-ului %d\n", i);
                exit(-1);
            }
        } else {
            reduce_arguments[k].id = k;
            reduce_arguments[k].reduce = reduce_number;
            reduce_arguments[k].results = map_results;
            reduce_arguments[k].barrier = &barrier;

		    int r = pthread_create(&reduce_threads[k], NULL,
                                    f_reduce, &reduce_arguments[k]);

            if (r) {
                printf("Eroare la crearea thread-ului %d\n", k);
                exit(-1);
            } else
                k++;
        }
	}

    k = 0;
	for (int i = 0; i < map_number + reduce_number; i++) {
        if (i < map_number) {
		    int r = pthread_join(map_threads[i], &status);

            if (r) {
                printf("Eroare la asteptarea MAP - thread-ului %d\n", i);
                exit(-1);
		    }
        } else {
		    int r = pthread_join(reduce_threads[k], &status);

            if (r) {
                printf("Eroare la asteptarea REDUCE - thread-ului %d\n", k);
                exit(-1);
		    }
            k++;
        }
	}

    map_results->clear();
    map_results->shrink_to_fit();
    delete map_results;

    pthread_mutex_destroy(&read_mutex);
    pthread_mutex_destroy(&write_mutex);
    pthread_barrier_destroy(&barrier);
    
    return 0;
}