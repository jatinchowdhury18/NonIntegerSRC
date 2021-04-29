#pragma once

#include "BaseSRC.h"
#include "src_utils/HPFilters.h"

class HPResampler : public BaseSRC
{
public:
    HPResampler() = default;

    void prepare (double sample_rate, int block_size, double src_ratio) override
    {
        ratio = (float) src_ratio;
        Ts = 1.0f / ((float) sample_rate);
        Ts_in = 1.0f / ((float) sample_rate * ratio);
        Ts_out = 1.0f / ((float) sample_rate / ratio);
        y_old = 0.0f;
        
        inFilter = std::make_unique<InputFilterBank> (Ts);
        inFilter->set_freq ((float) sample_rate * 0.5f);
        inFilter->set_delta (Ts_in);

        outFilter = std::make_unique<OutputFilterBank> (Ts_in);
        outFilter->set_freq ((float) sample_rate * 0.5f);
        outFilter->set_delta (Ts_out);
        H0 = 1.0f; // outFilter->calcH0();

        middle.resize (int (block_size * ratio) + 1, 0.0f);
    }

#if 0 // use input filter (Dirac)
    int process (const float* input, float* output, int num_samples) override
    {
        inFilter->set_time (tn);
        int count = 0;
        for (int i = 0; i < num_samples; ++i)
        {
            while (tn < Ts)
            {
                inFilter->calcG();
                auto sum = FastMath::vSum(SSEComplexMulReal(inFilter->Gcalc, inFilter->x));
                output[count++] = -0.95097f * sum;

                tn += Ts_in;
            }
            tn -= Ts;

            inFilter->process(input[i]);
        }

        return count;
    }
#else // use output filter (rectangular)
    int process (const float* input, float* output, int num_samples) override
    {
        outFilter->set_time (tn);
        int count = 0;
        for (int i = 0; i < num_samples; )
        {
            SSEComplex xOutAccum;
            int counter = 0;
            while (tn < Ts)
            {
                auto y = input[i++];
                auto delta = (y - y_old);
                y_old = y;

                outFilter->calcG();
                xOutAccum += outFilter->Gcalc * delta;
                tn += Ts_out;
            }
            tn -= Ts;

            outFilter->process(xOutAccum);
            float sum = FastMath::vSum(xOutAccum._r);
            output[count++] = y_old + sum;
        }

        return count;
    }
#endif

private:
    std::unique_ptr<InputFilterBank> inFilter;
    std::unique_ptr<OutputFilterBank> outFilter;

    float Ts = 1.0f;
    float Ts_in = 1.0f;
    float Ts_out = 1.0f;
    float tn = 0.0;

    float y_old = 0.0f;
    float ratio = 1.0f;

    float H0 = 1.0f;

    std::vector<float> middle;
};
