// STL includes
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <conio.h>
#include <cstring>

// External API inlcudes
#include "./libs/portaudio/include/portaudio.h"

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 512 // Can be 1024, essentially the size of the buffer, "after I get 512 of these, go into callback function"

// High level function to check the error state of a PortAudio instance
static void checkError(PaError theError)
{
    if (theError != paNoError)
    {
        // Print resultant port audio error
        printf("PortAudio Error: %s\n", Pa_GetErrorText(theError));
        exit(EXIT_FAILURE);
    }
}

static inline float max(float a, float b)
{
    return (a > b) ? a : b;
}

static inline float absolute(float a)
{
    return a > 0 ? a : -a;
}

static int paTestCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags statusFlags, void* userData)
{
    float* in = (float*)inputBuffer;
    (void)outputBuffer;

    int displaySize = 100;
    printf("\r");


    float vol_l, vol_r = 0;

    for (unsigned long i = 0; i < framesPerBuffer * 2; i+=2)
    {
        vol_l = max(vol_l, absolute(in[i]));
        vol_r = max(vol_l, absolute(in[i+1]));
    }

    // Display volume bar
    for(int i = 0; i < displaySize; i++)
    {
        float barProportion = i / (float)displaySize;
        if (barProportion <= vol_l && barProportion <= vol_r)
        {
            printf("█");
        }
        else if(barProportion <= vol_l)
        {
            printf("▀");
        }
        else if(barProportion <= vol_r)
        {
            printf("▄");
        }
        else
        {
            printf(" "); 
        }
    }

    fflush(stdout);

    return 0;
}

int main()
{
    // Assume successful operation until error flagged
    int32_t mainReturn = EXIT_SUCCESS;

    // Create Port Audio instance and initialise
    PaError paError = Pa_Initialize();
    checkError(paError);
    
    // Validate number of devices
    const int numDevices = Pa_GetDeviceCount();
    if (numDevices < 0)
    {
        printf("Error getting device count: %s", Pa_GetErrorText(numDevices));
        mainReturn = EXIT_FAILURE;
    }
    else if (numDevices == 0)
    {
        printf("No devices available for audio\n");
    }
    else
    {
        // Iterate over available input devices and print their information
        for (uint32_t i = 0U; i< numDevices; i++)
        {
            const PaDeviceInfo* paDeviceInfo = Pa_GetDeviceInfo(i);
            if (paDeviceInfo != NULL)
            {
                printf("Device #%d\n", i+1);
                printf("Name : %s\n", paDeviceInfo->name);
                printf("Max Input Channels : %d\n", paDeviceInfo->maxInputChannels);
                printf("Max Output Channels : %d\n", paDeviceInfo->maxOutputChannels);
                printf("Default Sample Rate : %f\n", paDeviceInfo->defaultSampleRate);
                
                // Check host API name
                const PaHostApiInfo* paHostApiInfo = Pa_GetHostApiInfo(paDeviceInfo->hostApi);
                if(paHostApiInfo != NULL)
                {
                    printf("Host API : %s\n", paHostApiInfo->name);
                    const bool isAsioDevice = (paHostApiInfo->name == "ASIO") ? true : false;
                    printf("Is ASIO Device : %d\n", isAsioDevice);
                }
            }
        }
        printf("Num Devices Available : %d\n", numDevices);

        // Naively using device 1, analog focusrite usb with 2 inputs
        const int inputDeviceNumber = 1;
        const int outputDeviceNumber = 4;
        // Set up input and output parameters, essentially just structs
        PaStreamParameters inputParameters;
        PaStreamParameters outputParameters;
        // Null these out
        memset(&inputParameters, 0, sizeof(inputParameters));
        memset(&outputParameters, 0, sizeof(outputParameters));
        // Set input parameters
        inputParameters.channelCount = 2;
        inputParameters.device = inputDeviceNumber;
        inputParameters.hostApiSpecificStreamInfo = NULL;
        inputParameters.sampleFormat = paFloat32;
        inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputDeviceNumber)->defaultLowInputLatency;
        // Set output parameters
        outputParameters.channelCount = 2;
        outputParameters.device = outputDeviceNumber;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        outputParameters.sampleFormat = paFloat32;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputDeviceNumber)->defaultLowInputLatency;

        // Include stream
        PaStream* stream;
        paError = Pa_OpenStream(&stream, &inputParameters, &outputParameters, SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag, paTestCallback, NULL);
        checkError(paError);

        // Start stream
        paError = Pa_StartStream(stream);
        checkError(paError);

        Pa_Sleep(5*1000);

        paError = Pa_StopStream(stream);

    }

    // Deallocate resources required for initialisation
    paError = Pa_Terminate();
    checkError(paError);
    printf("Resources successfully deallocated.");

    //getch();

    return mainReturn;
}