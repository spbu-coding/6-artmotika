#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "sortings.h"

#define CHAR_COUNT 256

void swap(char **str1, char **str2){
    char *tmp = *str2;
    *str2 = *str1;
    *str1 = tmp;
}

int compareFunc(const char* array1, const char* array2){
    if (strcmp(array1, array2) > 0) return 1;
    else return 0;
}

void free_results(char **results, unsigned int count) {
    for (unsigned int i = 0; i < count; ++i) {
        free(results[i]);
    }
    free(results);
    results = NULL;
}

int read_arguments(int argc, char** argv, int* name_sort_index, int* name_comparateur_index){
    if (argc != 6) {
        fprintf(stderr, "Incorrect number of arguments. Should be 5 arguments\n");
        return -1;
    }
    char *p = NULL;
    const char *size_pointer = argv[1];
    if(strtol(argv[1], &p, 10) < 0 || size_pointer == p){
        fprintf(stderr, "The first argument should be number > 0\n");
        return -1;
    }

    if(strstr(argv[2], ".txt") == NULL){
        fprintf(stderr, "Incorrect expansion\n");
        return -1;
    }
    if(strstr(argv[2], ".txt") == NULL){
        fprintf(stderr, "Incorrect expansion\n");
        return -1;
    }
    size_t count_of_sorts = 5;
    char names_sorts[5][10] = {"bubble", "insertion", "merge", "quick", "radix"};


    for (int i = 0; i < (int)count_of_sorts; ++i) {
        if (!strcmp(argv[4], names_sorts[i])){
            *name_sort_index = i;
            break;
        }
    }
    if (*name_sort_index == -1){
        fprintf(stderr, "Incorrect name of algorithm\n");
        return -1;
    }

    if (!strcmp(argv[5], "asc")){
        *name_comparateur_index = 0;
    }else if (!strcmp(argv[5], "des")){
        *name_comparateur_index = 1;
    }else{
        fprintf(stderr, "Incorrect name of comparator\n");
        return -1;
    }

    return 0;
}

int get_info_file(FILE* file, size_t max_length_of_string, strings_array_t sorting_strings, size_t number_of_sorting_elements) {
    size_t i;
    for (i = 0; i < number_of_sorting_elements && !feof(file); i++) {
        if(fgets(sorting_strings[i], max_length_of_string, file) == NULL) {
            fprintf(stderr, "Error in get_info_file() in fgets()\n");
            return -1;
        }
    }
    if(i < number_of_sorting_elements && feof(file)) {
        fprintf(stderr, "You specified more arguments than there are in the file\n");
        return -1;
    }
    else
        return 0;
}

int main(int argc, char** argv) {
    int name_sort_index = -1;
    int name_comparateur_index;
    if (read_arguments(argc, argv, &name_sort_index, &name_comparateur_index)) return -1;
    unsigned int number_of_sorting_elements = strtoul(argv[1], NULL, 10);

    strings_array_t sorting_strings = (strings_array_t) malloc(number_of_sorting_elements * sizeof(char*));
    if (sorting_strings == NULL) return -1;
    for (unsigned int i = 0; i < number_of_sorting_elements; ++i){
        sorting_strings[i] = (char*) malloc(MAX_INPUT_STRING_SIZE * sizeof(char));
        if (sorting_strings[i] == NULL){
            free_results(sorting_strings, i);
            fprintf(stderr,"Error to allocate memory");
            return -1;
        }
    }

    FILE *input_file = fopen (argv[2],"r");

    if (input_file == NULL){
        free_results(sorting_strings, number_of_sorting_elements);
        fprintf(stderr, "Error opening input file\n");
        return -1;
    }

    if (get_info_file(input_file, MAX_INPUT_STRING_SIZE, sorting_strings, number_of_sorting_elements)) {
        fclose(input_file);
        free_results(sorting_strings, number_of_sorting_elements);
        fprintf(stderr, "Error with reading input file\n");
        return -1;
    }

    fclose(input_file);
    //----------------------Sorting strings---------------------------
    if (number_of_sorting_elements > 0) {
        if (name_sort_index == 0) bubble(sorting_strings, number_of_sorting_elements, compareFunc);
        else if (name_sort_index == 1) insertion(sorting_strings, number_of_sorting_elements, compareFunc);
        else if (name_sort_index == 2) merge(sorting_strings, number_of_sorting_elements, compareFunc);
        else if (name_sort_index == 3) quick(sorting_strings, number_of_sorting_elements, compareFunc);
        else if (name_sort_index == 4) radix(sorting_strings, number_of_sorting_elements, compareFunc);
    }
    //---------------------------------------------------------------
    FILE *output_file = fopen (argv[3],"w");
    if (input_file == NULL){
        fclose(input_file);
        free_results(sorting_strings, number_of_sorting_elements);
        fprintf(stderr, "Error opening output file\n");
        return -1;
    }
    if(!number_of_sorting_elements){
        if(fputs("\n", output_file) == EOF){
            fprintf(stderr, "Error opening output file\n");
            return -1;
        }
    }else {
        if (!name_comparateur_index) {
            for (size_t i = 0; i < number_of_sorting_elements; i++) {
                if (fputs(sorting_strings[i], output_file) != EOF) {
                    if (strcspn(sorting_strings[i], "\n") == strlen(sorting_strings[i])) {
                        if (fputs("\n", output_file) == EOF) {
                            fprintf(stderr, "Unable to write to file\n");
                            return -1;
                        }
                    }
                } else {
                    fprintf(stderr, "Unable to write to file");
                    return -1;
                }
            }
        }else{
            for (int i = (int)number_of_sorting_elements-1; i >= 0; --i){
                if (fputs(sorting_strings[i], output_file) != EOF) {
                    if (strcspn(sorting_strings[i], "\n") == strlen(sorting_strings[i])) {
                        if (fputs("\n", output_file) == EOF) {
                            fprintf(stderr, "Unable to write to file\n");
                            return -1;
                        }
                    }
                } else {
                    fprintf(stderr, "Unable to write to file");
                    return -1;
                }
            }
        }
    }

    fclose(output_file);
    free_results(sorting_strings, number_of_sorting_elements);
    return 0;
}

void bubble(strings_array_t array, array_size_t size, comparator_func_t compare_func){
    for (unsigned int i = 0; i < size - 1; ++i) {
        for (unsigned int j = 0; j < size - i - 1; ++j) {
            int result_compareFunc = compare_func(array[j], array[j+1]);
            if (result_compareFunc){
                swap(&array[j], &array[j+1]);
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

void merge_wrapper2(strings_array_t array, int left, int middle, int right, comparator_func_t compare_func){
    int lsize = middle - left + 1;
    int rsize = right - middle;
    char *lbuff[lsize], *rbuff[rsize];
    for(int i = 0; i < lsize; i++) lbuff[i] = array[left + i];
    for(int j = 0; j < rsize; j++) rbuff[j] = array[middle + 1 + j];
    int i = 0;
    int j = 0;
    int k = left;
    while(i < lsize && j < rsize){
        if(!compare_func(lbuff[i], rbuff[j])){
            array[k] = lbuff[i];
            i++;
        }
        else{
            array[k] = rbuff[j];
            j++;
        }
        k++;
    }
    while(i < lsize){
        array[k] = lbuff[i];
        i++;
        k++;
    }
    while(j < rsize){
        array[k] = rbuff[j];
        j++;
        k++;
    }
}

void merge_wrapper1(strings_array_t array, int left, int right, comparator_func_t compare_func){
    if(left >= right) return;
    int middle = (left + right - 1) / 2;
    merge_wrapper1(array, left, middle, compare_func);
    merge_wrapper1(array, middle + 1, right, compare_func);
    merge_wrapper2(array, left, middle, right, compare_func);
}

void merge(strings_array_t array, array_size_t size, comparator_func_t compare_func){
    merge_wrapper1(array, 0, (int)size - 1, compare_func);
}

int quick_wrapper2(strings_array_t array, int left, int right, comparator_func_t compare_func){
    char *p = array[right];
    int i = (left - 1);

    for(int j = left; j <= right - 1; j++){
        if(!compare_func(array[j], p)){
            i++;
            swap(&array[i], &array[j]);
        }
    }
    swap(&array[i + 1], &array[right]);
    return (i + 1);
}

void quick_wrapper1(strings_array_t array, int left, int right, comparator_func_t compare_func){
    if(left < right){
        int middle = quick_wrapper2(array, left, right, compare_func);
        quick_wrapper1(array, left, middle - 1, compare_func);
        quick_wrapper1(array, middle + 1, right, compare_func);
    }
}

void quick(strings_array_t array, array_size_t size, comparator_func_t compare_func){
    quick_wrapper1(array, 0, (int) size - 1, compare_func);
}

void radix(strings_array_t array, array_size_t size, comparator_func_t compare_func){
    const bool is_asc = (!compare_func("a", "b"));
    array_size_t max_length = 0;
    for(size_t i = 0; i < size; i++){
        array_size_t length = strlen(array[i]);
        if(max_length < length) max_length = length;
    }
    if(max_length == 0) return;
    size_t buff[CHAR_COUNT];
    strings_array_t tmp = (strings_array_t)(malloc(size * sizeof(char *)));
    for(size_t i = max_length; i > 0; i--){
        size_t counter = 0;
        for(size_t j = 0; j < CHAR_COUNT; j++) buff[j] = 0;
        for(size_t j = 0; j < size; j++) buff[(unsigned char)(array[j][i - 1])]++;
        if(is_asc){
            for(size_t j = 0; j < CHAR_COUNT; j++){
                size_t value = buff[j];
                buff[j] = counter;
                counter += value;
            }
        }
        else{
            for(size_t j = CHAR_COUNT; j > 0; j--){
                size_t value = buff[j - 1];
                buff[j - 1] = counter;
                counter += value;
            }
        }
        for(size_t j = 0; j < size; j++){
            tmp[buff[(unsigned char)array[j][i - 1]]] = array[j];
            buff[(unsigned char)array[j][i - 1]]++;
        }
        for(size_t j = 0; j < size; j++) array[j] = tmp[j];
    }
    free(tmp);
}