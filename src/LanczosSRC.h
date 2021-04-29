#pragma once

#include "BaseSRC.h"
#include "src_utils/LanczosResampler.h"
#include <memory>

class LanczosSRC : public BaseSRC
{
public:
    LanczosSRC() = default;

    void prepare (double sample_rate, int block_size, double src_ratio) override
    {
        ratio = src_ratio;
        lanczos = std::make_unique<LanczosResampler> ((float) sample_rate, float (sample_rate * ratio));
    }

    int process (const float* input, float* output, int num_samples) override
    {
        lanczos->renormalizePhases();

        for (int i = 0; i < num_samples; ++i)
            lanczos->push(input[i]);

        return lanczos->populateNext (output, int (num_samples * ratio + 1));
    }

private:
    std::unique_ptr<LanczosResampler> lanczos;
    double ratio = 1.0;
};
