// ./main < data.txt o=n o=i v=x f=p k=w u=f y=b k=u j=v k=g j=q j=z k=y
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 65535
#define N_FREQS 26

typedef struct {
    char c;
    float freq;
} freq_item_t;

/**
 * @brief Frequency of letters in English alphabet
 */
freq_item_t freqs[N_FREQS] = {
    {'A', 0.0817},
    {'B', 0.0150},
    {'C', 0.0278},
    {'D', 0.0425},
    {'E', 0.1270},
    {'F', 0.0223},
    {'G', 0.0202},
    {'H', 0.0609},
    {'I', 0.0697},
    {'J', 0.0015},
    {'K', 0.0077},
    {'L', 0.0403},
    {'M', 0.0241},
    {'N', 0.0657},
    {'O', 0.0751},
    {'P', 0.0193},
    {'Q', 0.0010},
    {'R', 0.0599},
    {'S', 0.0633},
    {'T', 0.0906},
    {'U', 0.0276},
    {'V', 0.0098},
    {'W', 0.0236},
    {'X', 0.0015},
    {'Y', 0.0197},
    {'Z', 0.0007},
};

/**
 * @brief Convert uppercase letter to an array index
 */
static inline unsigned int freq_idx(char c)
{
    int r = c - 'A';
    return (r >= 0 && r < N_FREQS) ? r : -1;
}

/**
 * @brief Compare letter frequencies (used in qsort)
 */
int cmp_freqs(const void *a, const void *b)
{
    return (((freq_item_t*)a)->freq - ((freq_item_t*)b)->freq) * -10000;
}

/**
 * @brief Swap two letters specified in cmdline options
 * @details Usage: ./binary oldchar=newchar oldchar=newchar ...
 */
void swap_letters(char oc, char nc)
{
    freq_item_t *oldc = NULL;
    freq_item_t *newc = NULL;
    freq_item_t tc;

    for(unsigned int i = 0; i < N_FREQS; i++) {
        if(oldc != NULL && newc != NULL)
            break;
        if(freqs[i].c == oc)
            oldc = &freqs[i];
        if(freqs[i].c == nc)
            newc = &freqs[i];
    }

    tc = *oldc;
    *oldc = *newc;
    *newc = tc;
}

/**
 * @brief Sort freqs array by its frequency field and switch between characters
 *        if requested
 */
void prepare_freqs(int argc, char *argv[])
{
    qsort(freqs, N_FREQS, sizeof(*freqs), cmp_freqs);

    char oc, nc;
    oc = nc = '\0';

    for(int i = 1; i < argc; i++) {
        if(sscanf(argv[i], "%c=%c", &oc, &nc) == 2 &&
           isalpha(oc) && isalpha(nc)) {
            oc = toupper(oc);
            nc = toupper(nc);
            swap_letters(oc, nc);
        } else {
            fprintf(stderr, "%s oldchar=newchar oldchar=newchar ...\n",
                    argv[0]);
            exit(1);
        }
    }
}

int main(int argc, char *argv[])
{
    prepare_freqs(argc, argv);

    int c, idx, bidx = 0;
    char buffer[BUFFER_SIZE] = {0,};
    freq_item_t letters[N_FREQS];

    // Read input data and count frequency of each letter
    memset(letters, 0, sizeof(*letters) * N_FREQS);
    while((c = fgetc(stdin)) != EOF && bidx < BUFFER_SIZE -1) {
        c = toupper(c);
        idx = freq_idx(c);
        if(idx >= 0) {
            letters[idx].freq++;
            letters[idx].c = c;
        }

        buffer[bidx++] = c;
    }

    buffer[bidx] = '\0';
    // Sort input alphabet
    qsort(letters, N_FREQS, sizeof(*letters), cmp_freqs);

    // Replace each input letter by equivalent letter from freqs array
    for(unsigned int i = 0; buffer[i] != '\0' && i < BUFFER_SIZE; i++) {
        for(unsigned int j = 'A'; j <= 'Z'; j++) {
            if(letters[freq_idx(j)].c == '\0')
                continue;

            idx = freq_idx(buffer[i]);
            if(idx >= 0) {
                if(buffer[i] == letters[freq_idx(j)].c) {
                    buffer[i] = freqs[freq_idx(j)].c;
                    break;
                }
            }
        }
    }

    buffer[bidx] = '\0';
    printf("%s", buffer);

    return 0;
}
