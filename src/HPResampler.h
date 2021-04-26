#pragma once

#include "hp_utils/HPFilters.h"

class HPResampler : public BaseSRC
{
public:
    HPResampler() = default;

    void prepare (double sample_rate, int block_size, double src_ratio) override
    {
        ratio = (float) src_ratio;
        Ts = 1.0f / ((float) sample_rate);
        Ts_up = 1.0f / ((float) sample_rate * ratio);
        Ts_down = 1.0f / ((float) sample_rate / ratio);
        y_old = 0.0f;
        
        upFilter = std::make_unique<InputFilterBank> (Ts);
        downFilter = std::make_unique<OutputFilterBank> (Ts);

        upFilter->set_freq ((float) sample_rate * 0.5f);
        upFilter->set_delta (Ts_up);

        downFilter->set_freq ((float) sample_rate * 0.5f);
        downFilter->set_delta (Ts_up);

        H0 = downFilter->calcH0();
    }

    int process (const float* input, float* output, int num_samples) override
    {
        if (ratio > 1.0f) // upsample
        {
            upFilter->set_time (tn);
            int count = 0;
            for (int i = 0; i < num_samples; ++i)
            {
                while (tn < Ts)
                {
                    upFilter->calcG();
                    auto sum = FastMath::vSum(SSEComplexMulReal(upFilter->Gcalc, upFilter->x));
                    output[count++] = -0.95097f * sum;

                    tn += Ts_up;
                }
                tn -= Ts;

                upFilter->process(input[i]);
            }

            return count;
        }

        if (ratio < 1.0f) // downsample
        {
            downFilter->set_time (tn);
            int count = 0;
            for (int i = 0; i < num_samples; )
            {
                SSEComplex xOutAccum;
                while (tn < Ts)
                {
                    auto y = input[i++];
                    auto delta = y - y_old;
                    y_old = y;
                    downFilter->calcG();
                    xOutAccum += downFilter->Gcalc * delta;

                    tn += Ts_down;
                }
                tn -= Ts;

                downFilter->process(xOutAccum);
                float sum = FastMath::vSum(xOutAccum._r);
                output[count++] = y_old + sum;
            }

            return count;
        }

        // edge case
        std::copy (input, &input[num_samples], output);
        return num_samples;
    }

private:
    std::unique_ptr<InputFilterBank> upFilter;
    std::unique_ptr<OutputFilterBank> downFilter;

    float Ts = 1.0f;
    float Ts_up = 1.0f;
    float Ts_down = 1.0f;
    float tn = 0.0;

    float y_old = 0.0f;

    float ratio = 1.0f;
    float H0 = 1.0f;
};
