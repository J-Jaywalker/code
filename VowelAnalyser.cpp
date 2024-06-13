#include "VowelAnalyser.h"

VowelAnalyserBase::VowelAnalyserBase()
{
    InitialisePortAudio();
}

VowelAnalyserBase::~VowelAnalyserBase()
{
    // De-allocate resources initialised used by instance
    m_PortAudioErrorState = Pa_Terminate();
    VowelAnalyserBase::CheckError(m_PortAudioErrorState);
    printf("Resources successfully deallocated.\n");
}

void VowelAnalyserBase::InitialisePortAudio()
{
    m_PortAudioErrorState = Pa_Initialize();
    VowelAnalyserBase::CheckError(m_PortAudioErrorState);
        printf("PortAudio instance successfully initialised.\n");
}

int VowelAnalyserBase::GetDevicesAndCount(AudioDeviceType requiredDeviceType)
{
    const int devicesFound = Pa_GetDeviceCount();
    uint32_t matchingDevices {0U};
    // Validate number of devices first
    if (devicesFound < 0)
    {
        printf("Error getting device count: %s.", Pa_GetErrorText(devicesFound));
    }
    else if (devicesFound == 0)
    {
        printf("No devices available for audio.\n");
    }
    else
    {
        // Iterate through devices of relevant type
        for (uint32_t i = 0U; i< devicesFound; i++)
        {
            const PaDeviceInfo* paDeviceInfo = Pa_GetDeviceInfo(i);
            if (paDeviceInfo != NULL)
            {
                // Check device type is of relevant type
                AudioDeviceType deviceType = (AudioDeviceType)((paDeviceInfo->maxInputChannels > 0U) + (2U * (paDeviceInfo->maxOutputChannels > 0)));
                if (deviceType == requiredDeviceType)
                {
                    printf("Device ID: %d | Name : %s\n", i, paDeviceInfo->name);
                    printf("Max Channels : [ IN:%d / OUT:%d ]\n", paDeviceInfo->maxInputChannels, paDeviceInfo->maxOutputChannels);
                    printf("Sample Rate  : %f\n\n", paDeviceInfo->defaultSampleRate);
                    matchingDevices++;
                }
            }
        }
    }
    return matchingDevices;
}

void VowelAnalyserBase::StartStream(PaStream **stream, const PaStreamParameters *inputParameters, const PaStreamParameters *outputParameters, double sampleRate, unsigned long framesPerBuffer, PaStreamFlags streamFlags, PaStreamCallback *streamCallback, void *userData)
{
    // Open Stream
    m_PortAudioErrorState = Pa_OpenStream(stream, inputParameters, outputParameters, VowelAnalyserBase::SAMPLE_RATE , VowelAnalyserBase::FRAMES_PER_BUFFER, paNoFlag, AudioFrameCallback, NULL);
    CheckError(m_PortAudioErrorState);

    // Start stream
    m_PortAudioErrorState = Pa_StartStream(stream);
    CheckError(m_PortAudioErrorState);

    Pa_Sleep(5*1000);

    StopStream(stream);
}

void VowelAnalyserBase::StopStream(PaStream **stream)
{
    m_PortAudioErrorState = Pa_StopStream(stream);
    CheckError(m_PortAudioErrorState);
}

/// @brief Checks the status of PortAudio instance and exits if errored
/// @param theError PaError of the PortAudio instance to check
void VowelAnalyserBase::CheckError(PaError theError)
{
    if (theError != paNoError)
    {
        // Print resultant port audio error
        printf("PortAudio Error: %s.\n", Pa_GetErrorText(theError));
        exit(EXIT_FAILURE);
    }
}

int VowelAnalyserBase::AudioFrameCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags statusFlags, void* userData)
{
        float* in = (float*)inputBuffer;
    (void)outputBuffer;

    int displaySize = 30;
    printf("\r");

    float vol_l, vol_r = 0;

    for (unsigned long i = 0; i < framesPerBuffer * 2; i+=2)
    {
        vol_l = VowelAnalyserBase::MaxOfTwoValues(vol_l, VowelAnalyserBase::AbsOfFloat(in[i]));
        vol_r = VowelAnalyserBase::MaxOfTwoValues(vol_l, VowelAnalyserBase::AbsOfFloat(in[i+1]));
    }

    // Display volume bar
    for(int i = 0; i < displaySize; i++)
    {
        float barProportion = i / (float)displaySize;
        if (barProportion <= vol_l && barProportion <= vol_r)
        {
            printf("X");
        }
        else if(barProportion <= vol_l)
        {
            printf("B");
        }
        else if(barProportion <= vol_r)
        {
            printf("C");
        }
        else
        {
            printf(" "); 
        }
    }

    fflush(stdout);

    return 0;
}
