#include <chrono>
#include <iostream>
#include <sndfile.h>
#include <utility>
#include <vector>

#include "LibSamplerRateSRC.h"
#include "HPResampler.h"

void usage()
{
    std::cout << "USAGE: src_sndfile in_file out_file target_sample_rate mode" << std::endl;
}

using Vec2d = std::vector<std::vector<float>>;

std::pair<Vec2d, double> load_file (char* file, SF_INFO& sf_info)
{
    std::cout << "Loading file: " << file << std::endl;

    auto wavFile = sf_open(file, SFM_READ, &sf_info);

    if (sf_info.frames == 0)
    {
        std::cout << "File could not be opened!" << std::endl;
        exit (1);
    }

    const double fs = (double) sf_info.samplerate;
    std::cout << "Original sample rate: " << fs << std::endl;

    std::vector<float> readInterleaved(sf_info.channels * sf_info.frames, 0.0);
    sf_readf_float(wavFile, readInterleaved.data(), sf_info.frames);
    sf_close(wavFile);

    Vec2d audio (sf_info.channels, std::vector<float> (sf_info.frames, 0.0));

    // de-interleave channels
    for (int i = 0; i < sf_info.frames; ++i)
    {
        int interleavedPtr = i * sf_info.channels;
        for(size_t ch = 0; ch < sf_info.channels; ++ch)
            audio[ch][i] = readInterleaved[interleavedPtr + ch];
    }

    return std::make_pair(audio, fs);
}

void write_file (char* file, const Vec2d& audio, double fs, SF_INFO& sf_info)
{
    std::cout << "Writing to file: " << file << std::endl;

    const auto channels = (int) audio.size();
    const auto frames = (sf_count_t) audio[0].size();
    sf_info.frames = frames;
    sf_info.samplerate = (int) fs;

    auto wavFile = sf_open(file, SFM_WRITE, &sf_info);
    std::vector<float> writeInterleaved(channels * frames, 0.0);

    // de-interleave channels
    for (int i = 0; i < frames; ++i)
    {
        int interleavedPtr = i * channels;
        for(int ch = 0; ch < channels; ++ch)
            writeInterleaved[interleavedPtr + ch] = audio[ch][i];
    }

    sf_writef_float(wavFile, writeInterleaved.data(), frames);
    sf_close(wavFile);
}

std::unique_ptr<BaseSRC> getSRC (int mode)
{
    if (mode == 0)
    {
        std::cout << "Using mode: libsamplerate" << std::endl;
        return std::make_unique<LibSampleRateSRC>();
    }
    
    if (mode == 1)
    {
        std::cout << "Using mode: HPResampler" << std::endl;
        return std::make_unique<HPResampler>();
    }

    std::cout << "Mode argument not recogized!" << std::endl;
    return std::make_unique<LibSampleRateSRC>();
}

Vec2d process_data (const Vec2d& input, double ratio, BaseSRC* src, float fs)
{
    constexpr int block_size = 2048;

    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double>;

    auto start = clock_t::now();

    Vec2d output;
    int ch = 0;
    for (auto& data : input)
    {
        std::cout << "Processing channel " << ch++ << "..." << std::endl;

        src->prepare ((double) fs, (int) input.size(), ratio);
        std::vector<float> out (int (data.size() * ratio + 1), 0.0f);

        src->process (data.data(), out.data(), (int) data.size());

        output.push_back(out);
    }
    
    auto dur = std::chrono::duration_cast<second_t>(clock_t::now() - start).count();
    std::cout << "Finished processing in " << dur << " seconds" << std::endl;

    return output;
}

int main (int argc, char* argv[])
{
    if (argc != 5)
    {
        usage();
        return 0;
    }

    SF_INFO sf_info;
    auto [audio, fs] = load_file (argv[1], sf_info);

    const auto target_fs = std::atof (argv[3]);
    const auto ratio = target_fs / fs;

    auto src = getSRC (std::atoi (argv[4]));
    auto audio_out = process_data (audio, ratio, src.get(), (float) fs);

    write_file (argv[2], audio_out, target_fs, sf_info);

    return 0;
}
