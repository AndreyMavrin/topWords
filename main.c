#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#define MAX_CHARS 26
#define MAX_WORD_SIZE 50

const char* FILENAME = "words.txt";
const int TOP = 10;

typedef struct TrieNode {
    bool isEnd;
    unsigned frequency;
    int indexMinHeap;
    struct TrieNode* child[MAX_CHARS];
} TrieNode;

typedef struct MinHeapNode {
    TrieNode* root;
    unsigned frequency;
    char* word;
} MinHeapNode;

typedef struct MinHeap {
    unsigned capacity;
    int count;
    MinHeapNode* array;
} MinHeap;

TrieNode* newTrieNode() {
    TrieNode* trieNode = (TrieNode*)malloc(sizeof(TrieNode));
    trieNode->isEnd = 0;
    trieNode->frequency = 0;
    trieNode->indexMinHeap = -1;
    for (int i = 0; i < MAX_CHARS; ++i) {
        trieNode->child[i] = NULL;
    }

    return trieNode;
}

MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->capacity = capacity;
    minHeap->count  = 0;
    minHeap->array = (MinHeapNode*)malloc(sizeof(MinHeapNode) * minHeap->capacity);

    return minHeap;
}

void swapMinHeapNodes (MinHeapNode* a, MinHeapNode* b) {
    MinHeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    int smallest = idx;
    if (left < minHeap->count && minHeap->array[left].frequency < minHeap->array[smallest].frequency) {
        smallest = left;
    }
    if (right < minHeap->count && minHeap->array[right].frequency < minHeap->array[smallest].frequency) {
        smallest = right;
    }
    if (smallest != idx) {
        minHeap->array[smallest].root->indexMinHeap = idx;
        minHeap->array[idx].root->indexMinHeap = smallest;
        swapMinHeapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->count - 1;
    for (int i =(n - 1) / 2; i >= 0; --i) {
        minHeapify(minHeap, i);
    }
}

void insertInMinHeap(MinHeap* minHeap, TrieNode** root, const char* word) {
    if ((*root)->indexMinHeap != -1) {
        minHeap->array[(*root)->indexMinHeap].frequency++;
        minHeapify(minHeap, (*root)->indexMinHeap);
    } else if (minHeap->count < minHeap->capacity) {
        int count = minHeap->count;
        minHeap->array[count].frequency = (*root)->frequency;
        minHeap->array[count].word = (char*)malloc(sizeof(char) * (strlen(word) + 1));
        strcpy(minHeap->array[count].word, word);
        minHeap->array[count].root = *root;
        (*root)->indexMinHeap = minHeap->count;
        minHeap->count++;
        buildMinHeap(minHeap);
    } else if ((*root)->frequency > minHeap->array[0].frequency) {
        minHeap->array[0].root->indexMinHeap = -1;
        minHeap->array[0].root = *root;
        minHeap->array[0].root->indexMinHeap = 0;
        minHeap->array[0].frequency = (*root)->frequency;
        free(minHeap->array[0].word);
        minHeap->array[0].word = (char*)malloc(sizeof(char) * (strlen(word) + 1));
        strcpy( minHeap->array[0].word, word);
        minHeapify(minHeap, 0);
    }
}

void insertUtil (TrieNode** root, MinHeap* minHeap, const char* word, const char* dupWord) {
    if (*root == NULL) {
        *root = newTrieNode();
    }
    if (*word != '\0') {
        insertUtil(&((*root)->child[tolower(*word)-97]), minHeap, word + 1, dupWord);
    } else {
        if ((*root)->isEnd) {
            (*root)->frequency++;
        } else {
            (*root)->isEnd = 1;
            (*root)->frequency = 1;
        }

        insertInMinHeap(minHeap, root, dupWord);
    }
}

void printKMostFreq(FILE* fp, int k) {
    MinHeap* minHeap = createMinHeap(k);
    TrieNode* root = (TrieNode*)malloc(sizeof(TrieNode));
    char buffer[MAX_WORD_SIZE];
    while (fscanf(fp, "%s", buffer) != EOF) {
        insertUtil(&root, minHeap, buffer, buffer);
    }

    for (int i = 0; i < minHeap->count; i++) {
        printf("%s : %d\n", minHeap->array[i].word, minHeap->array[i].frequency);
    }
}

int main() {
    FILE *fp;
    if ((fp = fopen(FILENAME, "r")) == NULL) {
        printf("Can't open file: %s", FILENAME);
        return -1;
    }

    printKMostFreq(fp, TOP);
    fclose(fp);
    return 0;
}
