/*
 ==============================================================================
 
 ADSR.h
 Created: 24 Jan 2018 7:06:35pm
 Author:  Roland Rabien
 
 ==============================================================================
 */

#include "adsr.h"
#include <cmath>

void ADSR::setAttack (float seconds)
{
    attackDelta = 1.0 / (seconds * sampleRate);
}

void ADSR::setDecay (float seconds)
{
    decayDelta = 1.0 / (seconds * sampleRate);
}

void ADSR::setRelease (float seconds)
{
    releaseDelta = 1.0 / (seconds * sampleRate);
}

void ADSR::setSustainLevel (float level)
{
    sustainLevel = level;
}

void ADSR::process (AudioSampleBuffer& buffer)
{
    process (buffer, 0, buffer.getNumSamples());
}

void ADSR::process (AudioSampleBuffer& buffer, int startSample, int numSamples)
{
    float* d = buffer.getWritePointer (0);
    for (int i = 0; i < numSamples; i++)
    {
        switch (state)
        {
            case idle:
                break;
            case attack:
                output += attackDelta;
                if (output >= 1.0)
                {
                    output = 1.0;
                    state = decay;
                }
                break;
            case decay:
                output -= decayDelta;
                if (output <= sustainLevel)
                {
                    output = sustainLevel;
                    state = sustain;
                }
                break;
            case sustain:
                break;
            case release:
                output -= releaseDelta;
                if (output <= 0.0)
                {
                    output = 0.0;
                    state = idle;
                }
                break;
        }
        *d++ = output;
    }
}

float ADSR::process()
{
    switch (state)
    {
        case idle:
            break;
        case attack:
            output += attackDelta;
            if (output >= 1.0)
            {
                output = 1.0;
                state = decay;
            }
            break;
        case decay:
            output -= decayDelta;
            if (output <= sustainLevel)
            {
                output = sustainLevel;
                state = sustain;
            }
            break;
        case sustain:
            break;
        case release:
            output -= releaseDelta;
            if (output <= 0.0)
            {
                output = 0.0;
                state = idle;
            }
            break;
    }
    return output;
}

void ADSR::reset()
{
    state = idle;
    output = 0.0f;
}
