#ifndef NETALGORITHM_HPP
#define NETALGORITHM_HPP

#include <stdlib.h>
#include <ctime>

float fRand(const float i, const float f);
float avgError(std::vector<float>& error);
float sigmoid(std::vector<float>& input);
float sigmoid(float input);
float sum(std::vector<float>& input);
float average(std::vector<float>& input);

template<typename T> void vwrite(std::vector<T> &V, FILE* outFile){
    unsigned int L = V.size();
    if(L < 1) return;
    fwrite(&L, sizeof(unsigned int), 1, outFile);
    T tVect[L];
    for(unsigned int i = 0; i < L; ++i){
        tVect[i] = V[i];
    }
    fwrite(tVect, sizeof(T), L, outFile);
}

template<typename T> std::vector<T> vread(FILE* inFile){
    unsigned int L(0);
    std::vector<T> output;
    fread(&L, sizeof(unsigned int), 1, inFile);
    T tVect[L];
    fread(tVect, sizeof(T), L, inFile);
    for(size_t i = 0; i < L; ++i){
        output.push_back(tVect[i]);
    }
    return output;
}

#endif // NETALGORITHM_HPP
