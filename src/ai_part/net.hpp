#ifndef NET_HPP
#define NET_HPP

#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <cmath>

#include "netAlgorithm.hpp"

class neuralObj{
public:
    virtual ~neuralObj(){ }
};

enum class NeuralType{ INPUT, OUTPUT, HIDDEN, STRUCT, MEMORY };

std::string ntype_str(NeuralType type);

struct memoryItem{

    std::vector<unsigned char> memV;

    ~memoryItem(){ }
};

class neuralLayer;

class neuron: public neuralObj{
private:
    float getSigmoidInput();
    float getMemoryInput();

public:
    memoryItem* memory;

    neuron():
        type(NeuralType::HIDDEN),
        weight(0.0f),
        bias(1.0f){ };
    neuron(const float W, const float B, NeuralType Ntype):
        type(Ntype),
        weight(W), bias(B){
        if(Ntype == NeuralType::MEMORY) memory = new memoryItem; }

    NeuralType type;

    std::vector<float> inWeights;
    std::vector<float> outWeights;

    neuralLayer* parentLayer;

    std::vector<neuralLayer*> inLayers;
    std::vector<neuralLayer*> outLayers;

    std::vector<neuron*> output;
    std::vector<neuron*> input;

    float weight;
    float bias;

    void connect_to(neuron* other, const float W = 0.0f);
    void connect_from(neuron* other, const float W = 0.0f);

    void disconnect_from(neuron* other, int dir = -1, bool weights = true);
    void disconnect(int dir, bool weights = true);

    void branch(const unsigned int i, const float W = 0.0f, const float B = 0.0f, NeuralType type = NeuralType::HIDDEN);

    void inFunction();

    void print(bool full = false);

    ~neuron(){ }
};

class neuralLayer: public neuralObj{
private:
    void print(neuralLayer* focus);

public:

    neuralLayer(){ }
    neuralLayer(const unsigned int i, const float W = 0.0f, const float B = 0.0f, NeuralType type = NeuralType::HIDDEN);

    size_t size(){ return neurons.size(); }

    std::vector<neuron*> neurons;

    inline neuron operator[](const unsigned int i){ return *neurons[i]; }
    inline neuron* operator()(const unsigned int i){ return neurons[i]; }

    void extend(const unsigned int newLength, const float W = 0.0f, NeuralType type = NeuralType::HIDDEN);
    void shrink(const unsigned int newLength);

    void connect_to(neuron* other, const float W = 0.0f);
    void connect_from(neuron* other, const float W = 0.0f);

    void connect_to(neuralLayer* other, const float W = 0.0f);
    void connect_from(neuralLayer* other, const float W = 0.0f);

    inline neuralLayer* outBranch(const unsigned int i, const unsigned int N = 0){ return neurons[i]->outLayers[N]; }
    inline neuralLayer* inBranch(const unsigned int i, const unsigned int N = 0){ return neurons[i]->inLayers[N]; }

    void print(bool full = false);

    ~neuralLayer(){  }

};

class memBank;

class neuralNet: public neuralObj{
private:
    bool initialized;
    void correct(neuralLayer* focus, std::vector<float>& target, const float lrnR);

public:
    neuralLayer* primary;
    memBank* dataset;

    std::vector<unsigned int> levels;
    std::vector<unsigned int> scanDim;
    std::vector<unsigned int> Pos;

    float lifeTimeError;
    float lastTrainError;
    unsigned int numTrains;

    float getOutputError(std::vector<float>& target);

    neuralNet():
        initialized(false),
        primary(nullptr), dataset(nullptr), lifeTimeError(0.0f),
        lastTrainError(0.0f), numTrains(0), connected(false){ }
    neuralNet(std::vector<unsigned int> lvlLengths, memBank* data = nullptr, const unsigned int tracedepth = 2,
              std::vector<unsigned int> scanDim = {0}, std::vector<unsigned int> initPos = {0,0}):
        initialized(false),
        primary(new neuralLayer(lvlLengths.size(), 0.0f, 1.0f, NeuralType::STRUCT)),
        dataset(data), levels(lvlLengths), scanDim(scanDim), Pos(initPos),
        lifeTimeError(0.0f), lastTrainError(0.0f), numTrains(0), connected(data != nullptr){
            assemble();
            if(data != nullptr) connect_from(data, 2);
        }
    neuralNet(std::vector<unsigned int> lvlLengths, std::vector<unsigned int> scanDim, std::vector<unsigned int> initPos):
        initialized(false),
        primary(new neuralLayer(lvlLengths.size(), 0.0f, 1.0f, NeuralType::STRUCT)),
        dataset(nullptr), levels(lvlLengths), scanDim(scanDim), Pos(initPos),
        lifeTimeError(0.0f), lastTrainError(0.0f), numTrains(0), connected(false){
            assemble();
        }
    neuralNet(const char* filename, memBank* data = nullptr):
        initialized(false),
        scanDim({0}),
        Pos({0,0}),
        lifeTimeError(0.0f),
        lastTrainError(0.0f),
        numTrains(0),
        dataset(nullptr),
        connected(false)
        {
            load(filename);
            if(data != nullptr){
                connect_from(data, 2);
            }
        }

    void assemble();
    void assemble(std::vector<unsigned int>& lvlLengths);

    void connect_from(memBank* data, const unsigned int traceDepth = 2);
    void disconnect();
    bool connected;

    void randLayer(const unsigned int layer, const float i, const float f, int dir = 0);

    void propagate(neuron* focus);
    void correct(std::vector<float>& target, const float lrnR);
    void perceive();

    void shift(std::vector<unsigned int> newPos);
    void train(std::string baseFileName, std::vector<float>& target,
               std::vector<std::string>& targetFiles,
                 std::vector<std::string>& nonTargetFiles,
                 bool print = true);
    void analyze(std::string baseFileName, std::vector<float>& target,
               std::vector<std::string>& targetFiles,
                 std::vector<std::string>& nonTargetFiles,
                 bool print = false);
    float getOutput();

    void save(const char* filename);
    void load(const char* filename);

    void print(bool full = false);
    void printOutput();

    ~neuralNet(){ }
};

struct BMPHeader{
    unsigned char type[2];
    unsigned int size;
    unsigned char Length[8];
    unsigned int hSize;
    unsigned int imgWidth;
    unsigned int imgHeight;
    uint16_t planes;
    uint16_t bitCount;
    unsigned int compression;
    unsigned int fSize;
    unsigned int xRes;
    unsigned int yRes;
    unsigned int clrUsed;
    unsigned int clrRes;
};

struct RGBc{
    unsigned char R, G, B;
};

class memBank: public neuralObj{
private:
    void endTrace(neuralLayer* focus, std::vector<neuron*>& endV,
                  std::vector<unsigned int>& scanDim,
                  std::vector<unsigned int>& shift,
                  const unsigned int level);
public:
    neuralLayer* primary;

    std::vector<unsigned int> dimensions;

    memBank():primary(nullptr){ };
    memBank(const char* filename, const char* type = "bmp"):
        primary(nullptr){
            if(std::string(type) == "bmp") loadbmp(filename);
        }

    std::vector<neuron*> endTrace(std::vector<unsigned int> scanDim,
                                  std::vector<unsigned int> shift = {0,0},
                                  const unsigned int tracedepth = 2);

    void loadbmp(const char* filename);
    void loadbin(const char* filename);
    void load64(const char* filename);

    void save(const char* filename);

    ~memBank(){ }

};


#endif // NET_HPP
