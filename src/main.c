#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sortings.h"

#define ASCII_COUNT 128

int compareFunc(const char* array1, const char* array2){
    if (strcmp(array1, array2) > 0) return 1;
    else return 0;
}

void free_results(char **results, unsigned int count) {
    for (unsigned int i = 0; i < count; ++i) {
        free(results[i]);
    }
    free(results);
}

void fill_in_string(char* string, strings_array_t sorting_strings, unsigned int count_index){
    for (unsigned long i = 0; i < strlen(string); ++i){
        sorting_strings[count_index][i] = string[i];
    }
}

int compare_strings(char* string1, char* string2){
    if (strlen(string1) == strlen(string2)){
        for (unsigned long i = 0; i < strlen(string1); ++i){
            if (string1[i] != string2[i]) return 0;
        }
        return 1;
    }
    return 0;
}


int read_arguments(int argc, char** argv, int* name_sort_index, int* name_comparateur_index){
    if (argc != 6) {
        fprintf(stderr, "Incorrect number of arguments");
        return -1;
    }
    if (!atoi(argv[1])){
        fprintf(stderr, "The first argument should be number");
        return -1;
    }
    for (int j = 2; j < 4; ++j) {
        unsigned long index_expansion = 0;
        for (unsigned long i = 0; i < strlen(argv[j]); ++i) {
            if (argv[j][i] == '.') {
                index_expansion = i + 1;
                break;
            }
        }
        if ((index_expansion != 0) && (index_expansion + 3 == strlen(argv[j]))) {
            if (argv[j][index_expansion] != 't') {
                fprintf(stderr, "Incorrect expansion");
                return -1;
            }
            if (argv[j][index_expansion + 1] != 'x') {
                fprintf(stderr, "Incorrect expansion");
                return -1;
            }
            if (argv[j][index_expansion + 2] != 't') {
                fprintf(stderr, "Incorrect expansion");
                return -1;
            }
        } else {
            fprintf(stderr, "Incorrect expansion");
            return -1;
        }
    }

    char* names_sorts[5] = {"bubble", "insertion", "merge", "quick", "radix"};


    for (int i = 0; i < 5; ++i) {
        if (compare_strings(argv[4], names_sorts[i])) *name_sort_index = i;
    }
    if (*name_sort_index == -1) return -1;

    if (compare_strings(argv[5], "asc")){
        *name_comparateur_index = 0;
    }else if (compare_strings(argv[5], "des")){
        *name_comparateur_index = 1;
    }else return -1;

    return 0;
}

int main(int argc, char** argv) {
    int name_sort_index = -1;
    int name_comparateur_index;
    if (read_arguments(argc, argv, &name_sort_index, &name_comparateur_index)) return -1;
    unsigned int number_of_sorting_elements = atoi(argv[1]);

    strings_array_t sorting_strings = (strings_array_t) malloc(number_of_sorting_elements * sizeof(char*));
    if (sorting_strings == NULL) return -1;
    for (unsigned int i = 0; i < number_of_sorting_elements; ++i){
        sorting_strings[i] = (char*) malloc(MAX_INPUT_STRING_SIZE * sizeof(char));
        if (sorting_strings[i] == NULL) return -1;
    }

    FILE *input_file;
    char str[MAX_INPUT_STRING_SIZE];

    char *estr;
    input_file = fopen (argv[2],"r");

    if (input_file == NULL){
        fprintf(stderr, "Error opening file\n");
        return -1;
    }

    unsigned int count_index = 0;
    while (1){
        estr = fgets(str, sizeof(str), input_file);
        if (estr == NULL){
            if (feof(input_file) != 0){
                break;
            }
            else{
                fprintf(stderr, "Error reading file");
                return -1;
            }
        }
        fill_in_string(str, sorting_strings, count_index);
        count_index ++;
    }

    if (fclose(input_file) == EOF){
        fprintf(stderr, "Error to close the file");
        return -1;
    }
    //---------------------------------------------------------------

    if (name_sort_index == 0) bubble(sorting_strings, number_of_sorting_elements, compareFunc);
    else if (name_sort_index == 1) insertion(sorting_strings, number_of_sorting_elements, compareFunc);
    else if (name_sort_index == 2) merge(sorting_strings, number_of_sorting_elements, compareFunc, 0,  number_of_sorting_elements);
    else if (name_sort_index == 3) quick(sorting_strings, number_of_sorting_elements, compareFunc, 0, number_of_sorting_elements-1);
    else if (name_sort_index == 4) radix(sorting_strings, number_of_sorting_elements, compareFunc);

    //---------------------------------------------------------------
    FILE *output_file;
    output_file = fopen (argv[3],"w");
    if (input_file == NULL){
        fprintf(stderr, "Error opening file\n");
        return -1;
    }
    if (name_comparateur_index){
        for (int i = number_of_sorting_elements-1; i >= 0; --i){
            fwrite(sorting_strings[i], sizeof(char), strlen(sorting_strings[i]), output_file);
        }
    }else{
        for (unsigned int i = 0; i < number_of_sorting_elements; ++i){
            fwrite(sorting_strings[i], sizeof(char), strlen(sorting_strings[i]), output_file);
        }
    }


    if (fclose(output_file) == EOF){
        fprintf(stderr, "Error to close the file");
        return -1;
    }
    free_results(sorting_strings, number_of_sorting_elements);
    return 0;
}


void bubble(strings_array_t array, array_size_t size, comparator_func_t compare_func){
    char* tmp;
    for (unsigned int i = 0; i < size - 1; ++i) {
        for (unsigned int j = 0; j < size - i - 1; ++j) {
            int result_compareFunc = compare_func(array[j], array[j+1]);
            if (result_compareFunc){
                tmp = array[j];
                array[j] = array[j+1];
                array[j+1] = tmp;
            }
        }
    }
}

void insertion(strings_array_t array, array_size_t size, comparator_func_t compare_func){
    char* tmp;
    for (unsigned int i = 1; i < size; ++i){
        tmp = array[i];
        unsigned int j = i;
        while (j >= 1 && compare_func(array[j-1], tmp)){
            array[j] = array[j-1];
            j --;
        }
        array[j] = tmp;
    }
}

void merge_arrays(strings_array_t array, int left, int middle, int right){
    int it1 = 0, it2 = 0;
    char* results[right - left];

    while (left + it1 < middle && middle + it2 < right){
        if (compareFunc(array[middle + it2], array[left + it1])){
            results[it1 + it2] = array[left + it1];
            it1 ++;
        }else{
            results[it1 + it2] = array[middle + it2];
            it2 ++;
        }
    }

    while (left + it1 < middle){
        results[it1 + it2] = array[left + it1];
        it1 ++;
    }

    while (middle + it2 < right){
        results[it1 + it2] = array[middle + it2];
        it2 ++;
    }

    for (int i = 0; i < it1 + it2; ++i){
        array[left + i] = results[i];
    }
}

void merge(strings_array_t array, array_size_t size, comparator_func_t compare_func, int left, int right){
    if (left + 1 >= right)
        return;
    int middle = (left + right) / 2;
    merge(array, size, compare_func, left, middle);
    merge(array, size, compare_func, middle, right);
    merge_arrays(array, left, middle, right);
}

void quick(strings_array_t array, array_size_t size, comparator_func_t compare_func, int first, int last){
    if (first < last){
        int left = first, right = last;
        char* middle = array[(left+right)/2];
        do{
            while (compare_func(middle, array[left])) left++;
            while (compare_func(array[right], middle)) right--;
            if (left <= right){
                char* tmp = array[left];
                array[left] = array[right];
                array[right] = tmp;
                left++;
                right--;
            }
        }while (left <= right);
        quick(array, size, compare_func, first, right);
        quick(array, size, compare_func, left, last);

    }
}
void radix(strings_array_t array, array_size_t size, comparator_func_t compare_func){
    size_t length[size], max_length = 0;
    for (unsigned int i = 0; i < size; i++) {
        length[i] = strlen(array[i]) - 1;
        if (length[i] > max_length) {
            max_length = length[i];
        }
    }
    for (int i = (int)max_length - 1; i >= 0; i--) {
        unsigned int char_counter[ASCII_COUNT] = {0};
        for (unsigned int j = 0; j < size; j++) {
            if ((int)length[j] - 1 >= i) {
                char_counter[(unsigned int)array[j][i]]++;
            } else {
                char_counter[0]++;
            }
        }
        for (unsigned int j = 1; j < ASCII_COUNT; j++) {
            char_counter[j] += char_counter[j - 1];
        }
        char* buffer[size];
        size_t buffer_length[size];
        for (int j = (int)size - 1; j >= 0; j--) {
            if ((int)length[j] - 1 >= i) {
                buffer[(char_counter[(unsigned int)array[j][i]]) - 1] = array[j];
                buffer_length[(char_counter[(unsigned int)array[j][i]]--) - 1] = length[j];
            } else {
                buffer[(char_counter[0]) - 1] = array[j];
                buffer_length[(char_counter[0]--) - 1] = length[j];
            }
        }
        memcpy(array, buffer, size * sizeof(char *));
        memcpy(length, buffer_length, size * sizeof(size_t));
    }
}