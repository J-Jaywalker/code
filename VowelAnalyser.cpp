// STL includes
#include <stdlib.h>
#include <stdio.h>
#include "./libs/portaudio/include/portaudio.h"

static void checkError(PaError theError)
{
    if (theError != paNoError)
    {
        // Print resultant port audio error
        printf("Port Audio Error: %s\n", Pa_GetErrorText(theError));
        exit(EXIT_FAILURE);
    }
}

int main()
{
    PaError paError = Pa_Initialize();
    checkError(paError);
    printf("Resource allocation success, terminating PortAudio test allocation.\n");
    
    paError = Pa_Terminate();
    checkError(paError);
    printf("Resources successfulyl deallocated.");

    return EXIT_SUCCESS;
}