#include <chrono>
#include <iostream>
#include <vector>

#include "LibSamplerRateSRC.h"
#include "HPResampler.h"
#include "utils.h"
#include "../third_party/matplotlibcpp.h"

namespace
{
    constexpr int n_samples = 8192; // 100000;
    constexpr float freq = 100.0f;
    constexpr float fs = 48000.0f;
}

double process_data (const std::vector<float>& data, std::vector<float>& output, double ratio, BaseSRC* src)
{
    constexpr int block_size = 2048;
    src->prepare ((double) fs, block_size, ratio);
    
    std::vector out (int ((data.size() + block_size) * ratio + 1), 0.0f);

    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double>;

    auto start = clock_t::now();

    int out_ptr = 0;
    for (int i = 0; i + block_size < (int) data.size(); i += block_size)
        out_ptr += src->process (&data[i], &out[out_ptr], block_size);
    
    auto dur = std::chrono::duration_cast<second_t>(clock_t::now() - start).count();

    output.resize (out_ptr, 0.0f);
    std::copy (out.begin(), out.begin() + out_ptr, output.begin());

    return dur;
}

void test_libsamplerate (const std::vector<float>& data, double ratio)
{
    LibSampleRateSRC src;
    std::vector<float> output;
    auto time = process_data (data, output, ratio, &src);
    auto [latency, err] = calc_stats (freq, (float) ratio * fs, output);

    std::cout << "  Ratio: " << ratio << std::endl;
    std::cout << "  Duration: " << time << std::endl;
    // std::cout << "  Latency: " << latency << std::endl;
    // std::cout << "  Error: " << err << std::endl;

    // namespace plt = matplotlibcpp;
    // plt::plot(gen_time (fs, (int) data.size()), data);
    // plt::plot(gen_time (fs * (float) ratio, (int) output.size()), output);
    // plt::save("./libsamplerate.png");
}

void test_hpresampler (const std::vector<float>& data, double ratio)
{
    HPResampler src;
    std::vector<float> output;
    auto time = process_data (data, output, ratio, &src);
    auto [latency, err] = calc_stats (freq, (float) ratio * fs, output);

    // auto max_val = *std::max_element (output.begin(), output.end());
    // std::cout << max_val << std::endl;
    // std::cout << 1.0f / max_val << std::endl;

    std::cout << "  Ratio: " << ratio << std::endl;
    std::cout << "  Duration: " << time << std::endl;
    // std::cout << "  Latency: " << latency << std::endl;
    // std::cout << "  Error: " << err << std::endl;

    // namespace plt = matplotlibcpp;
    // plt::plot(gen_time (fs, (int) data.size()), data);
    // plt::plot(gen_time (fs * (float) ratio, (int) output.size()), output);
    // plt::save("./hpresampler.png");
}

int main()
{
    std::cout << "Generating input data..." << std::endl;
    auto sine = gen_sine (freq, fs, n_samples);

    std::cout << "Testing libsamplerate:" << std::endl;
    test_libsamplerate (sine, 2.0);
    test_libsamplerate (sine, 0.5);
    test_libsamplerate (sine, 1.088);
    test_libsamplerate (sine, 0.919);

    std::cout << "Testing Holters-Parker Resampler:" << std::endl;
    test_hpresampler (sine, 2.0);
    test_hpresampler (sine, 0.5);
    test_hpresampler (sine, 1.088);
    test_hpresampler (sine, 0.919);

    return 0;
}
