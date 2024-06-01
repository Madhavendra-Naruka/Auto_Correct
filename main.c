#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_WORD_LENGTH 100
#define DICTIONARY_SIZE 5000

// Function to calculate Levenshtein distance
int levenshtein_distance(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int matrix[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) {
        matrix[i][0] = i;
    }
    for (int j = 0; j <= len2; j++) {
        matrix[0][j] = j;
    }

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            matrix[i][j] = fmin(matrix[i - 1][j] + 1,
                               fmin(matrix[i][j - 1] + 1,
                                    matrix[i - 1][j - 1] + cost));
        }
    }

    return matrix[len1][len2];
}

// Function to read dictionary words from a file
int read_dictionary(const char *filename, char dictionary[][MAX_WORD_LENGTH]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open dictionary file");
        return -1;
    }

    int count = 0;
    while (count < DICTIONARY_SIZE && fgets(dictionary[count], MAX_WORD_LENGTH, file)) {
        dictionary[count][strcspn(dictionary[count], "\n")] = '\0'; // Remove newline character
        count++;
    }

    fclose(file);
    return count;
}

// Function to check if two words have 75% matching characters
bool is_75_percent_match(const char *word1, const char *word2) {
    int len1 = strlen(word1);
    int len2 = strlen(word2);

    int match_count = 0;
    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++) {
            if (word1[i] == word2[j]) {
                match_count++;
                break;
            }
        }
    }

    float match_percentage = (float)match_count / len1;
    return match_percentage >= 0.75;
}

// Function to find the closest word(s) in the dictionary
void find_closest_words(const char *word, char dictionary[][MAX_WORD_LENGTH], int dict_size) {
    int min_distance = MAX_WORD_LENGTH;
    char closest_word[MAX_WORD_LENGTH]; // Only storing one word now
    int word_len = strlen(word);
    int found = 0;

    for (int i = 0; i < dict_size; i++) {
        int dict_word_len = strlen(dictionary[i]);

        // Check if the word is an exact match
        if (strcmp(word, dictionary[i]) == 0) {
            printf("Correct Spelling\n", word);
            return;
        }

        // Check if the first letter matches
        if (dictionary[i][0] != word[0]) {
            continue;
        }

        // Check if the dictionary word length is +1 or -1 of the input word length
        if (!(dict_word_len == word_len + 1 ||  dict_word_len == word_len)) {
            continue;
        }

        // Check if the words have 75% matching characters
        if (!is_75_percent_match(word, dictionary[i])) {
            continue;
        }

        int distance = levenshtein_distance(word, dictionary[i]);
        if (distance < min_distance) {
            min_distance = distance;
            strcpy(closest_word, dictionary[i]); // Store only the closest word
            found = 1;
        }
    }

    if (found) {
        printf("Auto-Correct: %s \n", closest_word);
    } else {
        printf("No matching word found.\n");
    }
}


int main() {
    char dictionary[DICTIONARY_SIZE][MAX_WORD_LENGTH];
    int dict_size = read_dictionary("dictionary.txt", dictionary);
    if (dict_size == -1) {
        return 1;
    }

    char word[MAX_WORD_LENGTH];
    printf("Enter a word: ");
    scanf("%s", word);

    find_closest_words(word, dictionary, dict_size);

    return 0;
}