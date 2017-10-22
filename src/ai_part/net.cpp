#include <vector>
#include <iostream>

#include "net.hpp"


std::string ntype_str(NeuralType type){
    switch(type){
        case NeuralType::INPUT: return "Input";
        case NeuralType::OUTPUT: return "Output";
        case NeuralType::HIDDEN: return "Hidden";
        case NeuralType::STRUCT: return "Structural";
        case NeuralType::MEMORY: return "Memory";
        default: return "Unknown";
    }
}

void neuron::connect_to(neuron* other, const float W){
    for(size_t i = 0; i < output.size(); ++i){
        if(output[i] == other) return;
    }
    output.push_back(other);
    other->connect_from(this);
    while(outWeights.size() < output.size()) outWeights.push_back(W);
}

void neuron::connect_from(neuron* other, const float W){
    for(size_t i = 0; i < input.size(); ++i){
        if(input[i] == other) return;
    }
    input.push_back(other);
    other->connect_to(this);
    while(inWeights.size() < input.size()) inWeights.push_back(W);
}

void neuron::print(bool full){
    std::cout << " -[" << ntype_str(type) << " Neuron - W: " << weight << " B: " << bias;
    if(type == NeuralType::MEMORY){
        std::cout << " M: {";
        for(size_t i = 0; i < memory->memV.size(); ++i){
            std::cout << (int)memory->memV[i];
            if(memory->memV.size() > 1 && i < memory->memV.size() - 1) std::cout << ',';
        }
        std::cout << '}';
    }
    std::cout << "]- ";
    if(full){
        std::cout << "\n\n\t>> Inputs:\n";
        for(size_t i = 0; i < input.size(); ++i){
            std::cout << "\t\t"; input[i]->print(false); std::cout << "\t|\tConn. Weight: " << inWeights[i] << std::endl;
        }
        std::cout << "\n\t>> Outputs:\n";
        for(size_t i = 0; i < output.size(); ++i){
            std::cout << "\t\t"; output[i]->print(false); std::cout << "\t|\tConn. Weight: " << outWeights[i] << std::endl;
        }
        std::cout << std::endl;
    }
}

void neuron::disconnect_from(neuron* other, int dir, bool weights){
    if(dir <= 0){
        for(size_t i = 0; i < input.size(); ++i){
            if(input[i] == other){
                input.erase(input.begin() + i);
                if(weights) inWeights.erase(inWeights.begin() + i);
                other->disconnect_from(this, -dir, weights);
            }
        }
    }
    if(dir >= 0){
        for(size_t i = 0; i < output.size(); ++i){
            if(output[i] == other){
                output.erase(output.begin() + i);
                if(weights) outWeights.erase(outWeights.begin() + i);
                other->disconnect_from(this, -dir, weights);
            }
        }
    }
}

void neuron::branch(const unsigned int i, const float W, const float B, NeuralType type){
    neuralLayer* newBranch = new neuralLayer(i, W, B, type);
    newBranch->neurons[0]->inLayers.push_back(parentLayer);
    newBranch->neurons[0]->connect_from(this);
    outLayers.push_back(newBranch);
}

void neuron::disconnect(int dir, bool weights){
    if(dir >= 0){
        for(size_t i = 0; i < input.size(); ++i){
            disconnect_from(output[i], dir, weights);
        }
    }
    else if(dir <= 0){
        for(size_t i = 0; i < input.size(); ++i){
            disconnect_from(input[i], dir, weights);
        }
    }
}

float neuron::getSigmoidInput(){
    std::vector<float> inSignal;
    for(size_t i = 0; i < input.size(); ++i){
        if((input[i]->type == NeuralType::INPUT) | (input[i]->type == NeuralType::HIDDEN)){
            inSignal.push_back(input[i]->weight * input[i]->bias * inWeights[i]);
        }
    }
    return sigmoid(inSignal);
}

float neuron::getMemoryInput(){
    int cWeight(0);
    float output(0.0f);
    for(size_t j = 0; j < input.size(); ++j){
        if(input[j]->type == NeuralType::MEMORY){
        for(size_t i = 0; i < input[j]->memory->memV.size(); ++i){
            cWeight += int(input[j]->memory->memV[i]);
        }
        output += (float)cWeight/255 * inWeights[j];
        }
    }
    return output;
}

void neuron::inFunction(){
    switch(type){
        case NeuralType::INPUT:{ weight = getMemoryInput(); break; }
        case NeuralType::HIDDEN:{ weight = getSigmoidInput(); break; }
        case NeuralType::OUTPUT:{ weight = (getSigmoidInput()+1)/2; break; }
        default: return; }
}

neuralLayer::neuralLayer(const unsigned int i, const float W, const float B, NeuralType type){
    for(unsigned int j = 0; j < i; ++j){
        neurons.push_back(new neuron(W, B, type));
        neurons[neurons.size() - 1]->parentLayer = this;
    }
}

void neuralLayer::connect_to(neuralLayer* other, const float W){
    for(size_t i = 0; i < size(); ++i){
        for(size_t j = 0; j < other->size(); ++j){
            neurons[i]->connect_to(other->neurons[j]);
        }
    }
}

void neuralLayer::connect_from(neuralLayer* other, const float W){
    for(size_t i = 0; i < size(); ++i){
        for(size_t j = 0; j < other->size(); ++j){
            neurons[i]->connect_from(other->neurons[j]);
        }
    }
}

void neuralLayer::extend(const unsigned int newLength, const float W, NeuralType type){
    while(size() < newLength){
        neurons.push_back(new neuron(W, 1.0f, type));
        neurons[neurons.size() - 1]->parentLayer = this;
    }
}

void neuralLayer::shrink(const unsigned int newLength){
    if(newLength <= size()) return;
    for(size_t i = size(); i >= newLength; --i){
        neurons[i]->disconnect(0, true);
    }
    neurons.erase(neurons.begin() + newLength, neurons.end());
}

void neuralLayer::print(bool full){
    std::cout << ">> Neural layer >> : " << this << " << ================================\n\n";
    for(size_t i = 0; i < size(); ++i){
        neurons[i]->print(full);
    }
    std::cout << "\n\n>> END: Neural layer >> : " << this << " << ===========================\n\n";
}

void neuralNet::assemble(){
    if(levels.size() > 0){
        if(levels[0] == 0 && dataset != nullptr){
            levels[0] = dataset->endTrace(scanDim, Pos, 2).size();
        }
        for(size_t L = 0; (L < levels.size()) && (L < primary->size()); ++L){
            NeuralType layertype;
            if(L == 0) layertype = NeuralType::INPUT;
            else if(levels.size() > 2 && L < levels.size() - 1) layertype = NeuralType::HIDDEN;
            else layertype = NeuralType::OUTPUT;

            (*primary)(L)->branch(levels[L], 0.0f, 1.0f, layertype);
            if(L > 0) primary->outBranch(L)->connect_from(primary->outBranch(L-1));
            randLayer(L, -2.4/levels[L], 2.4/levels[L]);
        }
    }
}

void neuralNet::assemble(std::vector<unsigned int>& lvlLengths){
    if(lvlLengths.size() > 0){
        if(primary->size() > lvlLengths.size()) primary->shrink(lvlLengths.size());
        else if(primary->size() < lvlLengths.size()) primary->extend(lvlLengths.size());

        levels = lvlLengths;

        for(size_t L = 0; L < levels.size(); ++L){
            NeuralType layertype;
            if(L == 0) layertype = NeuralType::INPUT;
            else if(levels.size() > 2 && L < 2) layertype = NeuralType::HIDDEN;
            else layertype = NeuralType::OUTPUT;

            for(size_t i = 0; i < primary->size(); ++i){
                (*primary)(i)->branch(levels[L], 0.0f, 1.0f, layertype);
            }

            if(L > 0) primary->outBranch(L)->connect_from(primary->outBranch(L-1));
        }
    }
}

void neuralNet::randLayer(const unsigned int layer, const float i, const float f, int dir){
    neuralLayer* focus = primary->outBranch(layer);
    for(size_t L = 0; L < focus->size(); ++L){
        neuron* N = (*focus)(L);
        if(dir <= 0){
            for(size_t j = 0; j < N->input.size(); ++j){
                N->inWeights[j] = fRand(i, f);
            }
        }
        if(dir >= 0){
            for(size_t j = 0; j < N->output.size(); ++j){
                N->outWeights[j] = fRand(i, f);
            }
        }
    }
}

void neuralNet::print(bool full){
    std::cout << ">> Neural Network >> " << this << " << =========================================\n\n";
    for(size_t i = 0; i < primary->size(); ++i){
        primary->outBranch(i)->print(full);
    }
    std::cout << ">> END: Neural Network >> " << this << " << ====================================\n\n";
}

void neuralNet::connect_from(memBank* data, const unsigned int traceDepth){
    std::vector<neuron*> endV = data->endTrace(scanDim, Pos, traceDepth);
    unsigned int eInd(0);
    neuralLayer* focus = primary->outBranch(0);
    for(size_t j = 0; (j < focus->size()) & (eInd < endV.size()) ; ++j){
        (*focus)(j)->connect_from(endV[eInd]);
        eInd++;
    }
    if(!initialized){
        neuralLayer* inputLayer = primary->outBranch(0);
        randLayer(0, -2.4/inputLayer->size(), 2.4/inputLayer->size(), -1);
        initialized = true;
    }
    dataset = data;
    connected = true;
}

void neuralNet::disconnect(){
    neuralLayer* focus = primary->outBranch(0);
    for(size_t j = 0; j < focus->size(); ++j){
        (*focus)(j)->disconnect(-1, false);
    }
    connected = false;
}

void neuralNet::propagate(neuron* focus){
    focus->inFunction();
    for(size_t i = 0; i < focus->output.size(); ++i){
        focus->output[i]->inFunction();
        propagate(focus->output[i]);
    }
}

void neuralNet::perceive(){
    neuralLayer* focus;
    for(size_t i = 0; i < primary->size(); ++i){
        focus = primary->outBranch(i);
        std::cout << focus->size() << "\n\n";
        for(size_t j = 0; j < focus->size(); ++j){
            focus->neurons[j]->inFunction();
        }
    }
}

void neuralNet::save(const char* filename){
    FILE* outFile = fopen(filename, "wb+");

    vwrite(levels, outFile);
    vwrite(scanDim, outFile);
    fwrite(&lifeTimeError, sizeof(float), 1, outFile);
    fwrite(&lastTrainError, sizeof(float), 1, outFile);
    fwrite(&numTrains, sizeof(unsigned int), 1, outFile);
    for(size_t i = 0; i < primary->size(); ++i){
        neuralLayer* focus = primary->outBranch(i);
        for(size_t j = 0; j < focus->size(); ++j){
            fwrite(&focus->neurons[j]->weight, sizeof(float), 1, outFile);
            fwrite(&focus->neurons[j]->bias, sizeof(float), 1, outFile);
            vwrite(focus->neurons[j]->inWeights, outFile);
            vwrite(focus->neurons[j]->outWeights, outFile);
        }
    }

    fclose(outFile);
}

void neuralNet::load(const char* filename){
    FILE* inFile = fopen(filename, "rb");

    levels = vread<unsigned int>(inFile);
    scanDim = vread<unsigned int>(inFile);
    fread(&lifeTimeError, sizeof(unsigned int), 1, inFile);
    fread(&lastTrainError, sizeof(unsigned int), 1, inFile);
    fread(&numTrains, sizeof(unsigned int), 1, inFile);

    primary = new neuralLayer(levels.size(), 0.0f, 1.0f, NeuralType::STRUCT);
    NeuralType type;
    for(size_t i = 0; i < levels.size(); ++i){
        if(i == 0) type = NeuralType::INPUT;
        else if((levels.size() > 2) & (i < levels.size() - 1)) type = NeuralType::HIDDEN;
        else type = NeuralType::OUTPUT;

        primary->neurons[i]->branch(levels[i], 0.0f, 1.0f, type);
        if(i > 0) primary->outBranch(i)->connect_from(primary->outBranch(i-1));
        neuralLayer* focus = primary->outBranch(i);
        for(size_t j = 0; j < levels[i]; ++j){
            fread(&focus->neurons[j]->weight, sizeof(float), 1, inFile);
            fread(&focus->neurons[j]->bias, sizeof(float), 1, inFile);
            focus->neurons[j]->inWeights = vread<float>(inFile);
            focus->neurons[j]->outWeights = vread<float>(inFile);
        }
    }

    fclose(inFile);
}

void neuralNet::shift(std::vector<unsigned int> newPos){
    disconnect();
    Pos = newPos;
    connect_from(dataset, 2);
}

float neuralNet::getOutputError(std::vector<float>& target){
    std::vector<float> error;
    neuralLayer* outputLayer = primary->outBranch(primary->size() - 1);
    for(size_t i = 0; ((i < outputLayer->size()) && (i < target.size())); ++i){
        error.push_back(outputLayer->neurons[i]->weight);
        error[i] = target[i] <= error[i] ? error[i] - target[i] : target[i] - error[i];
    }
    return average(error);
}

void neuralNet::correct(std::vector<float>& target, const float lrnR){
    float error = getOutputError(target);
    float res = 0.25f*lrnR;

    for(size_t i = primary->size(); ((error > res) && (i > 0)); --i){
        correct(primary->outBranch(i-1), target, lrnR);
        error = getOutputError(target);
    }
}

void neuralNet::correct(neuralLayer* focus, std::vector<float>& target, const float lrnR){

    const float res = 0.25f;

    for(size_t i = 0; i < focus->size(); ++i){
        if(focus->neurons[i]->type == NeuralType::OUTPUT) break; // Don't bias outputs
        float lastError = getOutputError(target);
        float inc = lastError*res*lrnR;
        focus->neurons[i]->bias += inc;
        propagate(focus->neurons[i]);
        float cError = getOutputError(target);
        if(cError > lastError){
            focus->neurons[i]->bias -= inc*2;
            propagate(focus->neurons[i]);
            lastError = cError;
            cError = getOutputError(target);
            if(cError > lastError){
                focus->neurons[i]->bias += inc;
                propagate(focus->neurons[i]);
            }
        }
    }

    if(getOutputError(target) > res/2){

    for(size_t i = 0; i < focus->size(); ++i){
        float lastError = getOutputError(target);
        float inc = lastError*res*lrnR/(focus->neurons[i]->input.size());
        for(size_t j = 0; j < focus->neurons[i]->input.size(); ++j){
            focus->neurons[i]->inWeights[j] += inc;
            propagate(focus->neurons[i]);
            float cError = getOutputError(target);
            if(cError > lastError){
                focus->neurons[i]->inWeights[j] -= inc*2;
                propagate(focus->neurons[i]);
                lastError = cError;
                cError = getOutputError(target);
                if(cError > lastError){
                    focus->neurons[i]->inWeights[j] += inc;
                    propagate(focus->neurons[i]);
                }
            }
        }
    }

    }
}

float neuralNet::getOutput(){
    return (*primary->outBranch(primary->size() - 1))(0)->weight;
}

void neuralNet::train(std::string baseDir, std::vector<float>& target,
                      std::vector<std::string>& targetFiles,
                      std::vector<std::string>& nonTargetFiles,
                      bool print){
    int numID = 1;
    std::string IDConv = std::to_string(numID);
    while(IDConv.size() < 3){ IDConv = '0' + IDConv; }
    std::string loadFileName;

    for(size_t i = 0; i < targetFiles.size(); ++i){
        loadFileName = baseDir + IDConv + '_' + targetFiles[i] + ".bmp";
        if(!access(loadFileName.c_str(), X_OK)){
            target[0] = 1.0f; goto Loop;
        }
    }
    for(size_t i = 0; i < nonTargetFiles.size(); ++i){
        loadFileName = baseDir + IDConv + '_' + nonTargetFiles[i] + ".bmp";
        if(!access(loadFileName.c_str(), X_OK)){
            target[0] = 0.0f; break;
        }
    }

    Loop:

    float tError(0.0f), res = 0.25f;
    unsigned int tRuns(0), nCorrect(0), nWrong(0), maxT(0), Tprog(0);

    memBank m;

    while(!access(loadFileName.c_str(), X_OK)){

        Tprog++;
        if(Tprog > maxT) maxT = Tprog;

        std::cout << std::endl;

        if(access(loadFileName.c_str(), R_OK)) continue;
        if(connected) disconnect();

        m.loadbmp(loadFileName.c_str());
        connect_from(&m, 2);
        perceive();

        float cError = getOutputError(target);
        tRuns++;
        tError += cError;
        float lrnR = log(1 + tError);

        if(print){
            std::cout << ">> File: " << loadFileName << "\t|\tAvg Error: " << tError/tRuns;
            std::cout << "\t|\tCorrect: " << (float)nCorrect*100.0f/(nCorrect+nWrong) << "\t|\tTarget: " << target[0] << "\r";
        }

        if(cError > res){
            unsigned int loopBoot = 0;
            while((cError > res) & (loopBoot < 5)){
                correct(target, lrnR);
                cError = getOutputError(target);
                loopBoot++;
                if(print){
                    std::cout << "\r>> File: " << loadFileName << "\t|\tAvg Error: " << tError/tRuns;
                    std::cout << "\t|\tCorrect: " << float(nCorrect)*100.0f/(nCorrect+nWrong) << "\t|\tTarget: " << target[0] << "\r";
                }
            }
        nWrong++;
        }
        else nCorrect++;



        numID++;
        IDConv = std::to_string(numID);

        while(IDConv.size() < 3){ IDConv = '0' + IDConv; }

        for(size_t i = 0; i < targetFiles.size(); ++i){
            loadFileName = baseDir + IDConv + '_' + targetFiles[i] + ".bmp";
            if(!access(loadFileName.c_str(), X_OK)){
                target[0] = 1.0f; goto endTrainLoop;
            }
        }
        for(size_t i = 0; i < nonTargetFiles.size(); ++i){
            loadFileName = baseDir + IDConv + '_' + nonTargetFiles[i] + ".bmp";
            if(!access(loadFileName.c_str(), X_OK)){
                target[0] = 0.0f; break;
            }
        }
        endTrainLoop: continue;
    }

    if(tRuns > 0){
        lifeTimeError = (lifeTimeError + tError/numTrains)/(numTrains+1);
        lastTrainError = tError/tRuns;
        numTrains++;
    }
}

void neuralNet::analyze(std::string baseDir, std::vector<float>& target,
                      std::vector<std::string>& targetFiles,
                      std::vector<std::string>& nonTargetFiles,
                      bool print){
    int numID = 1;
    std::string IDConv = std::to_string(numID);
    while(IDConv.size() < 3){ IDConv = '0' + IDConv; }
    std::string loadFileName;

    for(size_t i = 0; i < targetFiles.size(); ++i){
        loadFileName = baseDir + IDConv + '_' + targetFiles[i] + ".bmp";
        if(!access(loadFileName.c_str(), X_OK)){
            target[0] = 1.0f; goto Loop;
        }
    }
    for(size_t i = 0; i < nonTargetFiles.size(); ++i){
        loadFileName = baseDir + IDConv + '_' + nonTargetFiles[i] + ".bmp";
        if(!access(loadFileName.c_str(), X_OK)){
            target[0] = 0.0f; break;
        }
    }

    Loop:

    float tError(0.0f), res = 0.25f;
    unsigned int tRuns(0), nCorrect(0), nWrong(0), maxT(0), Tprog(0);

    memBank m;

    while(!access(loadFileName.c_str(), X_OK)){

        Tprog++;
        if(Tprog > maxT) maxT = Tprog;

        std::cout << std::endl;

        if(access(loadFileName.c_str(), R_OK)) continue;
        if(connected) disconnect();
        m.loadbmp(loadFileName.c_str());
        connect_from(&m, 2);
        perceive();

        float cError = getOutputError(target);
        tRuns++;
        tError += cError;
        tError /= tRuns;

        if(print){
            std::cout << ">> File: " << loadFileName << "\t|\tAvg Error: " << tError;
            std::cout << "\t|\tCorrect: " << (float)nCorrect*100.0f/(nCorrect+nWrong) << "\t|\tTarget: " << target[0] << "\r";
        }

        if(cError > res){
            nWrong++;
        }
        else nCorrect++;

        numID++;
        IDConv = std::to_string(numID);

        while(IDConv.size() < 3){ IDConv = '0' + IDConv; }

        for(size_t i = 0; i < targetFiles.size(); ++i){
            loadFileName = baseDir + IDConv + '_' + targetFiles[i] + ".bmp";
            if(!access(loadFileName.c_str(), X_OK)){
                target[0] = 1.0f; goto endTrainLoop;
            }
        }
        for(size_t i = 0; i < nonTargetFiles.size(); ++i){
            loadFileName = baseDir + IDConv + '_' + nonTargetFiles[i] + ".bmp";
            if(!access(loadFileName.c_str(), X_OK)){
                target[0] = 0.0f; break;
            }
        }
        endTrainLoop: continue;
    }
}


void neuralNet::printOutput(){
    std::cout << ">> Times Trained:\t" << numTrains << "\t|\tAvg. Error: " << lifeTimeError << "\t|\tCurrent Output: [";
    neuralLayer* outLayer = primary->outBranch(primary->size() - 1);
    for(size_t i = 0; i < outLayer->size(); ++i){
        std::cout << outLayer->neurons[i]->weight;
        if((outLayer->size() > 1) && (i < outLayer->size() - 1)) std::cout << ',';
    }
    std::cout << "]       \r";
}

std::vector<neuron*> memBank::endTrace(std::vector<unsigned int> scanDim, std::vector<unsigned int> shift,
                                       const unsigned int tracedepth){
    std::vector<neuron*> endV;
    if(scanDim[0] == 0) scanDim = dimensions;
    if(dimensions.size() < 2){
        for(size_t i = 0; i < primary->size(); ++i){
            endV.push_back(primary->neurons[i]);
        }
        return endV;
    }
    endTrace(primary, endV, scanDim, shift, tracedepth);
    return endV;
}

void memBank::endTrace(neuralLayer* focus, std::vector<neuron*>& endV,
                       std::vector<unsigned int>& scanDim,
                       std::vector<unsigned int>& shift, const unsigned int level){

    size_t iShift(shift[dimensions.size() - level]),
    fShift(shift[dimensions.size() - level] + scanDim[dimensions.size()] - level);

    if(iShift > dimensions[dimensions.size() - level] - scanDim[dimensions.size() - level]){
        iShift = dimensions[dimensions.size() - level] - scanDim[dimensions.size() - level];
    }
    if(fShift > dimensions[dimensions.size() - level]){
        fShift = dimensions[dimensions.size() - level];
    }

    for(size_t i = iShift; ((i < focus->size()) & (i < fShift)); ++i){
        if(((*focus)(i)->outLayers.size() > 0) && (level > 0)) endTrace(focus->outBranch(i), endV, scanDim, shift, level - 1);
        else endV.push_back(focus->neurons[i]);
    }
}

void memBank::loadbmp(const char* filename){
    FILE* inFile = fopen(filename, "rb");

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

    fread(type, sizeof(unsigned char), 2, inFile);
    fread(&size, sizeof(unsigned int), 1, inFile);
    fread(Length, sizeof(unsigned char), 8, inFile);
    fread(&hSize, sizeof(unsigned int), 1, inFile);
    fread(&imgWidth, sizeof(unsigned int), 1, inFile);
    fread(&imgHeight, sizeof(unsigned int), 1, inFile);
    fread(&planes, sizeof(unsigned char), 2, inFile);
    fread(&bitCount, sizeof(unsigned char), 2, inFile);
    fread(&compression, sizeof(unsigned int), 1, inFile);
    fread(&fSize, sizeof(unsigned int), 1, inFile);
    fread(&xRes, sizeof(unsigned int), 1, inFile);
    fread(&yRes, sizeof(unsigned int), 1, inFile);
    fread(&clrUsed, sizeof(unsigned int), 1, inFile);
    fread(&clrRes, sizeof(unsigned int), 1, inFile);

    dimensions = {imgHeight, imgWidth};

    unsigned int pixels = imgWidth*imgHeight, pInd(0);
    RGBc pixArray[pixels];
    fread(pixArray, sizeof(RGBc), pixels, inFile);

    if(primary != nullptr) delete(primary);
    primary = new neuralLayer(imgHeight, 0.0f, 1.0f, NeuralType::STRUCT);
    for(size_t i = 0; i < imgHeight; ++i){
        (*primary)(i)->branch(imgWidth, 0.0f, 1.0f, NeuralType::MEMORY);
        neuralLayer* focus = primary->outBranch(i);
        for(size_t j = 0; j < imgWidth; ++j){
            (*focus)(j)->memory->memV.push_back(pixArray[pInd].R);
            (*focus)(j)->memory->memV.push_back(pixArray[pInd].G);
            (*focus)(j)->memory->memV.push_back(pixArray[pInd].B);
            pInd++;
        }
    }

    fclose(inFile);
}

void memBank::load64(const char* filename){
    FILE* inFile = fopen(filename, "rb");

    fseek(inFile, 0L, SEEK_END);
    unsigned long long mSIZE = ftell(inFile);
    fseek(inFile, 0L, SEEK_SET);

    unsigned int length64 = mSIZE/4;
    std::cout << length64 << std::endl;

    if(primary != nullptr) delete(primary);
    primary = new neuralLayer(length64, 0.0f, 1.0f, NeuralType::MEMORY);
    unsigned int L;

    for(unsigned int i = 0; i < length64; ++i){
        fread(&L, sizeof(unsigned int), 1, inFile);
        primary->neurons[i]->memory->memV.push_back(L);
    }


}


