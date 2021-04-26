#pragma once

#include "BaseSRC.h"
#include <samplerate.h>
#include <algorithm>
#include <memory>

class LibSampleRateSRC : public BaseSRC
{
public:
    LibSampleRateSRC() = default;

    void prepare (double /*sample_rate*/, int block_size, double src_ratio) override
    {
        int error;
        src_state.reset (src_new (SRC_SINC_FASTEST, 1, &error));
        src_set_ratio (src_state.get(), src_ratio);

        ratio = src_ratio;
        // output_data.resize (block_size * src_ratio + 1, 0.0f); // allocate an extra sample for rounding error
    }

    int process (const float* input, float* output, int num_samples) override
    {
        SRC_DATA src_data {
            input,
            output,
            num_samples,
            int (num_samples * ratio + 1),
            0,
            0,
            0,
            ratio
        };

        src_process (src_state.get(), &src_data);

        return (int) src_data.output_frames_gen;
    }

private:
    double ratio = 1.0;
    std::unique_ptr<SRC_STATE, decltype(&src_delete)> src_state {nullptr, &src_delete};
};
