#include <cmath>
#include <vector>
#include <stdlib.h>
#include <iostream>

float fRand(float i, float f){
    float d = ((float) rand()*(f-i) / (RAND_MAX)+i);
    return d;
}

float sigmoid(std::vector<float>& input){
    float output(0.0), max(input[0]), min(input[0]), rG(0.0);
    for(size_t i = 0; i < input.size(); i++){
        if(input[i] > max) max = input[i];
        if(input[i] < min) min = input[i];
        output += input[i];
    }
    if(max-min == 0) rG = 1e16;
    else rG = 5.0/(max-min);
    output = (2.0/(1.0 + exp(-rG*output))) - 1.0;
    return output;
}

float sigmoid(float input){
    return 2.0/(1.0 + exp(-input)) - 1.0;
}

float avgError(std::vector<float>& error){
    float output(0.0f);
    for(size_t i = 0; i < error.size(); ++i){
        output += error[i] >= 0 ? error[i] : -error[i];
    }
    return output/error.size();
}

float sum(std::vector<float>& input){
    float output(0.0f);
    for(size_t i = 0; i < input.size(); ++i){
        output += input[i];
    }
    return output;
}

float average(std::vector<float>& input){
    float output(0.0f);
    for(size_t i = 0; i < input.size(); ++i){
        output += input[i];
    }
    return output / input.size();
}
