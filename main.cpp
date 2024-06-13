#include "VowelAnalyser.h"

int main()
{
    // Assume successful operation until error flagged
    int32_t mainReturn = EXIT_SUCCESS;
    // Input and output parameters
    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;

    // Initialise analyserBase, get intput devices
    VowelAnalyserBase analyserBase;
    printf("Getting available input devices...\n\n");
    const int inputDevices = analyserBase.GetDevicesAndCount(AudioDeviceType::INPUT);
    if (inputDevices > 0)
    {
        printf("Input devices available : %d\n", inputDevices);
        printf("Select audio input device ID : ");
        uint32_t inputDeviceSelected;
        std::cin >> inputDeviceSelected;

        // Set up input parameters - null first to initialise safely 
        memset(&inputParameters, 0, sizeof(inputParameters));
        inputParameters.channelCount = Pa_GetDeviceInfo(inputDeviceSelected)->maxInputChannels;
        inputParameters.device = inputDeviceSelected;
        inputParameters.hostApiSpecificStreamInfo = NULL;
        inputParameters.sampleFormat = paFloat32;
        inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputDeviceSelected)->defaultLowInputLatency;
    }
    else
    {
        exit(EXIT_FAILURE);
    }
    // Get output devices
    printf("\nGetting available output devices...\n\n");
    const int outputDevices = analyserBase.GetDevicesAndCount(AudioDeviceType::OUTPUT);
    if (outputDevices > 0)
    {
        printf("Output devices available : %d\n", outputDevices);
        printf("Select audio output device ID : ");
        uint32_t outputDeviceSelected;
        std::cin >> outputDeviceSelected;

        // Set up output parameters - null first to initialise safely 
        memset(&outputParameters, 0, sizeof(outputParameters));
        outputParameters.channelCount = Pa_GetDeviceInfo(outputDeviceSelected)->maxOutputChannels;
        outputParameters.device = outputDeviceSelected;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputDeviceSelected)->defaultLowOutputLatency;
    }
    else
    {
        exit(EXIT_FAILURE);
    }

    // Include stream
    PaStream* stream;
    analyserBase.StartStream(&stream, &inputParameters, &outputParameters, VowelAnalyserBase::SAMPLE_RATE , VowelAnalyserBase::FRAMES_PER_BUFFER, paNoFlag, analyserBase.AudioFrameCallback, NULL);

    analyserBase.StopStream(&stream);
    // // Start stream
    // paError = Pa_StartStream(stream);
    // VowelAnalyserBase::CheckError(paError);

    // Pa_Sleep(5*1000);

    // paError = Pa_StopStream(stream);

    return mainReturn;
}