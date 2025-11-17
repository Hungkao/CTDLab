#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 5000
#define MAX_STOP 500

typedef struct {
    char word[100];
    int lines[500];
    int lineCount;
} Entry;

Entry indexList[MAX_WORDS];
char stopWords[MAX_STOP][100];
int stopCount = 0;
int wordCount = 0;

/* =======================
     ĐỌC STOP WORDS
   ======================= */
void loadStopWords(FILE *fst) {
    char buffer[100];

    while (fgets(buffer, sizeof(buffer), fst)) {
        buffer[strcspn(buffer, "\n")] = 0;

        int i = 0, j = 0;
        char clean[100];

        while (buffer[i]) {
            if (isalpha(buffer[i])) {
                clean[j++] = tolower(buffer[i]);
            }
            i++;
        }
        clean[j] = '\0';

        if (strlen(clean) > 0) {
            strcpy(stopWords[stopCount++], clean);
        }
    }
}

/* ======================= */
int isStopWord(char *w) {
    for (int i = 0; i < stopCount; i++)
        if (strcmp(w, stopWords[i]) == 0)
            return 1;
    return 0;
}
/* ======================= */
int isProperNoun(const char *orig, char prev) {
    if (!isupper(orig[0])) return 0;
    if (prev == '.') return 0;
    return 1;
}
/* ======================= */
int findWord(char *w) {
    for (int i = 0; i < wordCount; i++)
        if (strcmp(indexList[i].word, w) == 0)
            return i;
    return -1;
}
/* ======================= */
void addLine(Entry *e, int line) {
    if (e->lineCount == 0 || e->lines[e->lineCount - 1] != line)
        e->lines[e->lineCount++] = line;
}
/* ======================= */
void addWord(char *w, int line) {
    int pos = findWord(w);

    if (pos == -1) {
        strcpy(indexList[wordCount].word, w);
        indexList[wordCount].lineCount = 0;
        addLine(&indexList[wordCount], line);
        wordCount++;
    } else {
        addLine(&indexList[pos], line);
    }
}
/* ======================= */
int cmp(const void *a, const void *b) {
    return strcmp(((Entry *)a)->word, ((Entry *)b)->word);
}
/* ======================= */


int main(int argc, char *argv[]) {

    if (argc != 4) {
        printf("Usage: %s <stopw.txt> <input.txt> <output.txt>\n", argv[0]);
        return 1;
    }

    FILE *fst = fopen(argv[1], "r");
    FILE *fin = fopen(argv[2], "r");
    FILE *fout = fopen(argv[3], "w");

    if (!fst || !fin) {
        printf("Khong mo duoc file dau vao!\n");
        return 1;
    }
    if (!fout) {
        printf("Khong mo duoc file output!\n");
        return 1;
    }

    loadStopWords(fst);

    char lineBuf[2000];
    int line = 0;
    char prev = '.';

    while (fgets(lineBuf, sizeof(lineBuf), fin)) {
        line++;

        int len = strlen(lineBuf);
        char word[100];
        int wpos = 0;

        for (int i = 0; i <= len; i++) {
            char c = lineBuf[i];

            if (isalpha(c)) {
                word[wpos++] = c;
            } else {
                if (wpos > 0) {
                    word[wpos] = '\0';

                    char orig[100];
                    strcpy(orig, word);

                    for (int j = 0; word[j]; j++)
                        word[j] = tolower(word[j]);

                    if (!isStopWord(word)) {
                        if (!isProperNoun(orig, prev)) {
                            addWord(word, line);
                        }
                    }
                    wpos = 0;
                }
            }

            if (!isspace(c))
                prev = c;
        }
    }

    fclose(fin);
    fclose(fst);

    qsort(indexList, wordCount, sizeof(Entry), cmp);

    fprintf(fout, "===== INDEX =====\n");
    for (int i = 0; i < wordCount; i++) {
        fprintf(fout, "%s ", indexList[i].word);
        for (int j = 0; j < indexList[i].lineCount; j++) {
            if (j > 0) fprintf(fout, ", ");
            fprintf(fout, "%d", indexList[i].lines[j]);
        }
        fprintf(fout, "\n");
    }

    fclose(fout);

    printf("Da ghi ket qua vao %s\n", argv[3]);

    return 0;
}
