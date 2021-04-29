#include <chrono>
#include <iostream>
#include <vector>

#include "LibSamplerRateSRC.h"
#include "HPResampler.h"
#include "LanczosSRC.h"
#include "utils.h"

#if MAKE_PLOTS
#include <matplotlibcpp.h>
namespace plt = matplotlibcpp;
#endif

namespace
{
    constexpr int n_samples = 1000000;
    constexpr float freq = 100.0f;
}

double process_data (const std::vector<float>& data, std::vector<float>& output, double ratio, BaseSRC* src, float fs)
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

void test_libsamplerate (const std::vector<float>& data, double ratio, float fs)
{
    LibSampleRateSRC src;
    std::vector<float> output;
    auto time = process_data (data, output, ratio, &src, fs);
    auto [latency, err] = calc_stats (freq, (float) ratio * fs, output);

    // std::cout << "  Ratio: " << ratio << std::endl;
    std::cout << "  libsamplerate Duration: " << time << std::endl;
    // std::cout << "  Latency: " << latency << std::endl;
    // std::cout << "  Error: " << err << std::endl;

#if MAKE_PLOTS
    plt::figure();
    plt::plot(gen_time (fs, (int) data.size()), data);
    plt::plot(gen_time (fs * (float) ratio, (int) output.size()), output);
    plt::xlim(0.0, 0.01);
    plt::save("./doc/libsamplerate.png");
#endif
}

void test_hpresampler (const std::vector<float>& data, double ratio, float fs)
{
    HPResampler src;
    std::vector<float> output;
    auto time = process_data (data, output, ratio, &src, fs);
    auto [latency, err] = calc_stats (freq, (float) ratio * fs, output);

    // auto max_val = *std::max_element (output.begin(), output.end());
    // std::cout << max_val << std::endl;
    // std::cout << 1.0f / max_val << std::endl;

    // std::cout << "  Ratio: " << ratio << std::endl;
    std::cout << "  HPResampler Duration: " << time << std::endl;
    // std::cout << "  Latency: " << latency << std::endl;
    // std::cout << "  Error: " << err << std::endl;

#if MAKE_PLOTS
    plt::figure();
    plt::plot(gen_time (fs, (int) data.size()), data);
    plt::plot(gen_time (fs * (float) ratio, (int) output.size()), output);
    plt::xlim(0.0, 0.01);
    plt::save("./doc/hpresampler.png");
#endif
}

void test_lanczos (const std::vector<float>& data, double ratio, float fs)
{
    LanczosSRC src;
    std::vector<float> output;
    auto time = process_data (data, output, ratio, &src, fs);
    auto [latency, err] = calc_stats (freq, (float) ratio * fs, output);

    // auto max_val = *std::max_element (output.begin(), output.end());
    // std::cout << max_val << std::endl;
    // std::cout << 1.0f / max_val << std::endl;

    // std::cout << "  Ratio: " << ratio << std::endl;
    std::cout << "  Lanczos Duration: " << time << std::endl;
    // std::cout << "  Latency: " << latency << std::endl;
    // std::cout << "  Error: " << err << std::endl;

#if MAKE_PLOTS
    plt::figure();
    plt::plot(gen_time (fs, (int) data.size()), data);
    plt::plot(gen_time (fs * (float) ratio, (int) output.size()), output);
    plt::xlim(0.0, 0.01);
    plt::save("./doc/lanczosresampler.png");
#endif
}

int main()
{
    std::cout << "Generating input data..." << std::endl;
    auto sine_48 = gen_sine (freq, 48000.0f, n_samples);
    auto sine_96 = gen_sine (freq, 96000.0f, n_samples);
    auto sine_441 = gen_sine (freq, 44100.0f, n_samples);

    std::cout << "Testing 48k -> 96k..." << std::endl;
    test_libsamplerate (sine_48, 2.0, 48000.0f);
    test_hpresampler (sine_48, 2.0, 48000.0f);
    test_lanczos (sine_48, 2.0, 48000.0f);

    std::cout << "Testing 96k -> 48k..." << std::endl;
    test_libsamplerate (sine_96, 0.5, 96000.0f);
    test_hpresampler (sine_96, 0.5, 96000.0f);
    test_lanczos (sine_96, 0.5, 96000.0f);

    std::cout << "Testing 44.1k -> 48k..." << std::endl;
    test_libsamplerate (sine_441, 1.088, 44100.0f);
    test_hpresampler (sine_441, 1.088, 44100.0f);
    test_lanczos (sine_441, 1.088, 44100.0f);

    std::cout << "Testing 48k -> 44.1k..." << std::endl;
    test_libsamplerate (sine_48, 0.919, 48000.0f);
    test_hpresampler (sine_48, 0.919, 48000.0f);
    test_lanczos (sine_48, 0.919, 48000.0f);

    return 0;
}
