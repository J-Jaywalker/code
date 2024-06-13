// STL includes
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <conio.h>
#include <cstring>
#include <iostream>
// External API inlcudes
#include "./libs/portaudio/include/portaudio.h"

enum AudioDeviceType : uint8_t
{
    INPUT = 1,
    OUTPUT = 2,
    INPUT_AND_OUTPUT = 3
};

class VowelAnalyserBase
{
public:
    /// @brief Constructor, currently unused
    VowelAnalyserBase();

    /// @brief Destructor
    ~VowelAnalyserBase();

    /// @brief Creates PortAudio instance and initialises it
    void InitialisePortAudio();

    /// @brief Gets and validates the number of audio devices of a particular type
    /// @param deviceType The type of audio device, can either be INPUT, OUTPUT or INPUT_AND_OUTPUT
    /// @return Number of devices, else <0 if error
    int GetDevicesAndCount(AudioDeviceType deviceType);

    void StartStream(PaStream **stream, const PaStreamParameters *inputParameters, const PaStreamParameters *outputParameters, double sampleRate, unsigned long framesPerBuffer, PaStreamFlags streamFlags, PaStreamCallback *streamCallback, void *userData);


    void StopStream(PaStream **stream);

    /// @brief Static method to check the error state of the PortAudio instance
    /// @param theError 
    static void CheckError(PaError theError);

    static int AudioFrameCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags statusFlags, void* userData);

    /// @brief Helper function - gets the maximum of two floating point numbers
    /// @param a First number to compare
    /// @param b Second number to compare
    /// @return Largest of the two numbers
    static inline float MaxOfTwoValues(float a, float b)
    {
        return (a > b) ? a : b;
    }

    /// @brief helper function - gets the absolute value of a floating point number
    /// @param a Float to get Abs of
    /// @return Abs value of float
    static inline float AbsOfFloat(float a)
    {
        return a > 0 ? a : -a;
    }

    // Defines the sample rate of input audio
    static constexpr double SAMPLE_RATE = 44100;
    // Defines max  frames be held inside buffer before audio callback function is executed
    static constexpr unsigned long FRAMES_PER_BUFFER = 512U;

private:
    // Member variables
    PaError m_PortAudioErrorState;

};