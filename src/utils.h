#include <math.h>
#include <algorithm>
#include <vector>

std::vector<float> gen_time (float fs, int num_samples)
{
    std::vector<float> time (num_samples);
    std::generate (time.begin(), time.end(), [=, n = 0.0f] () mutable { return n++ / fs; });

    return std::move (time);
}

std::vector<float> gen_sine (float freq, float fs, int num_samples)
{
    std::vector<float> data (num_samples);
    std::generate (data.begin(), data.end(), [=, n = 0.0f] () mutable { return std::sin (2.0f * M_PI * n++ * freq / fs); });

    return std::move (data);
}

int calc_latency (const std::vector<float>& data, const std::vector<float>& ref_data)
{
    auto find_first_point5 = [] (const std::vector<float>& x) -> int {
        for (int i = 0; i < (int) x.size(); ++i)
        {
            if (x[i] >= 0.5f)
                return i;
        }
        return 0;            
    };

    auto ref_one = find_first_point5 (ref_data);
    auto actual_one = find_first_point5 (data);

    return actual_one - ref_one;
}

std::pair<int, double> calc_stats (float freq, float fs, const std::vector<float>& data)
{
    const int num_samples = (int) data.size();
    auto compare_data = gen_sine (freq, fs, num_samples);
    auto latency_samp = calc_latency (data, compare_data);

    float error_sum = 0.0f;
    for (int i = 0; i < num_samples - latency_samp; ++i)
        error_sum += std::abs (compare_data[i] - data[i + latency_samp]);

    // for (int i = 0; i < 50; i += 5)
    //     std::cout << compare_data[i] << ", " << data[i] << std::endl;

    auto avg_error = error_sum / (float) num_samples;
    return std::make_pair (latency_samp, avg_error);
}
