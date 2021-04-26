# Non-Integer Sample Rate Conversion

This repository contains a comparison of sample-rate
conversion (SRC) algorithms, with an emphasis on performance
for non-integer SRC factors. Currently two oversampling
algorithms are implemented:

- libsamplerate (Sinc interpolation, using `SRC_SINC_FASTEST` mode)
- Holters-Parker Resampler (using 4th-order Butterworth filters)

## Results:
On my Linux machine, the HP resampler is faster by
the following amounts:

- 48 kHz -> 96 kHz: 20x faster
- 96 kHz -> 48 kHz: 20x faster
- 44.1 kHz -> 48 kHz: 12.5x faster
- 48 kHz -> 44.1 kHz: 16x faster

## Building
```bash
$ cmake -Bbuild
$ cmake --build build --config Release
```
Then to run the testing tool, run
`./build/src_test`.

## Credits

- [libsamplerate](https://github.com/libsndfile/libsamplerate)
- The HP-Resamples uses a bunch of code borrowed from the [Surge Synthesizer project](https://github.com/surge-synthsizer/surge) (in particular, the `SSEComplex` class written by Paul Walker)

## License

The code in this repository is licensed under the GPLv3. Enjoy!
