#pragma once

class BaseSRC
{
public:
    BaseSRC() = default;

    virtual void prepare (double sample_rate, int block_size, double src_ratio) = 0;
    virtual int process (const float* input, float* output, int num_samples) = 0;
};
