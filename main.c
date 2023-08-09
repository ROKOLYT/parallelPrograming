#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <math.h>
#include <omp.h>
#include <pthread.h>
#include <time.h>

long long int combinationsGenerated = 0;
long long int totalGenerated = 0;
long long int totalCombinations;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void generateCombinations(char* code, int codeLength, long long int startIdx, long long int endIdx) {
    char characters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int numCharacters = strlen(characters);

    long long int currentIndex = startIdx;

    for (long long int i = startIdx; i <= endIdx; i++) {
        long long int tempIndex = currentIndex;
        for (int j = codeLength - 1; j >= 0; j--) {
            code[j] = characters[tempIndex % numCharacters];
            tempIndex /= numCharacters;
        }
        code[codeLength] = '\0';  // Null-terminate the string

        // Increment the combinationsGenerated count
        pthread_mutex_lock(&mutex);
        combinationsGenerated++;
        pthread_mutex_unlock(&mutex);

        currentIndex++;
    }
}

void formatTime(int seconds) {
    int days, hours, minutes;

    days = seconds / (60 * 60 * 24);
    seconds -= days * (60 * 60 * 24);

    hours = seconds / (60 * 60);
    seconds -= hours * (60 * 60);

    minutes = seconds / 60;
    seconds -= minutes * 60;

    printf("Time left: %d days, %d hours, %d minutes, %d seconds\n", days, hours, minutes, seconds);
}

void* printCombinationsPerSecond(void* arg) {
    while (1) {
        Sleep(1000);  // Sleep for 1 second
        pthread_mutex_lock(&mutex);
        if (combinationsGenerated == 0) {
            return NULL;
        }
        else {
            totalGenerated += combinationsGenerated;
        }
        system("cls");
        printf("Combinations per second: %lld\n", combinationsGenerated);
        formatTime(((totalCombinations - totalGenerated) / combinationsGenerated));
        combinationsGenerated = 0;  // Reset count
        pthread_mutex_unlock(&mutex);
    }
}

int main() {
    int codeLength = 6;
    char code[codeLength + 1];

    totalCombinations = (long long int)pow(36, codeLength);
    int numThreads = omp_get_max_threads();
    long long int workPerThread = totalCombinations / numThreads;
    long long int remainingWork = totalCombinations % numThreads;

    pthread_t printThread;
    pthread_create(&printThread, NULL, printCombinationsPerSecond, NULL);

    #pragma omp parallel num_threads(numThreads)
    {
        int threadId = omp_get_thread_num();
        long long int startIndex = threadId * workPerThread + (threadId < remainingWork ? threadId : remainingWork);
        long long int endIndex = startIndex + workPerThread - 1 + (threadId < remainingWork ? 1 : 0);

        generateCombinations(code, codeLength, startIndex, endIndex);
    }

    pthread_join(printThread, NULL);

    return 0;
}