#include <vector>
#include <iostream>
#include <string>
#include <windows.h>
#include <stdlib.h>
#include <ctime>

#include "net.hpp"

float bit64Analyze(std::string byteStr, std::string filename){
    const int* c = (int*)byteStr.c_str();
    memBank m;
    unsigned int mSIZE = byteStr.size()/4;
    m.primary = new neuralLayer(mSIZE, 0.0f, 1.0f, NeuralType::MEMORY);
    for(size_t i = 0; i < m.primary->size(); ++i){
        m.primary->neurons[i]->memory->memV.push_back(c[i]);
    }
    neuralNet n(filename.c_str(), &m);
    n.perceive();
    return n.primary->outBranch(n.primary->size() - 1)->neurons[0]->weight;
}

void audioCorrect(std::string byteStr, float cValue, std::string filename){
    const int* c = (int*)byteStr.c_str();
    memBank m;
    unsigned int mSIZE = byteStr.size()/4;
    m.primary = new neuralLayer(mSIZE, 0.0f, 1.0f, NeuralType::MEMORY);
    for(size_t i = 0; i < m.primary->size(); ++i){
        m.primary->neurons[i]->memory->memV.push_back(c[i]);
    }
    neuralNet n(filename.c_str(), &m);
    n.perceive();
    std::vector<float> target = {cValue};
    n.correct(target, n.lifeTimeError);
    n.save(byteStr.c_str());
}

int main(int argc, char* argV[]){

    // Initialize seed for RNG

    srand(time(0));

    memBank m;
    m.load64("screen.bin");
    neuralNet n("NN.bin", &m);
    n.perceive();

    if(*argV[0] == 'g'){
        std::cout << "getting...\n";
        float f = n.getOutput();

        FILE* outFile = fopen("wb.bin", "wb+");
        fwrite(&f, sizeof(float), 1, outFile);

        fclose(outFile);
    }
    if(*argV[0] == 'l'){
        std::cout << "learning... \n";
        std::vector<float> target = {1.0f};
        float error = n.getOutputError(target);
        n.correct(target, n.lifeTimeError);
    }

    n.save("NN.bin");

    return 0;
}
