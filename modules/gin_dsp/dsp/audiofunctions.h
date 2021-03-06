/*
 ==============================================================================

 This file is part of the GIN library.
 Copyright (c) 2019 - Roland Rabien.

 ==============================================================================
 */


#pragma once

#include "audioutil.h"
#include "oscillators.h"

//==============================================================================
struct FuncState
{
    FuncState (double sr) : sampleRate (sr) {}
    virtual ~FuncState() = default;
    virtual void setSampleRate (double sr) { sampleRate = sr; }
    virtual void reset() {}
    double sampleRate = 44100.0;
};

//==============================================================================
struct OscState : public FuncState
{
    OscState (double sr) : FuncState (sr) {}

    float phase { Random::getSystemRandom().nextFloat() };

    inline void incPhase (float note)
    {
        if (note != lastNote)
        {
            lastNote = note;

            frequency = float (gin::getMidiNoteInHertz (note));
            const float period = 1.0f / frequency;
            const float periodInSamples = float (period * sampleRate);
            delta = 1.0f / periodInSamples;

            jassert (delta > 0);
        }
        phase += delta;
        if (phase > 1.0f)
            phase -= 1.0f;

        jassert (! std::isinf (phase));
    }

    void reset() override
    {
        phase = Random::getSystemRandom().nextFloat();
    }

    float lastNote = -1.0f, frequency = -1.0f, delta = -1.0f;
};

//==============================================================================
struct HP12State : public FuncState
{
    HP12State (double sr) : FuncState (sr) {}

    double process (double v, double freq, double q)
    {
        freq = jlimit (8.0, jmin (20000.0, sampleRate / 2.0), freq);
        q = jmax (0.0000001, q);

        auto c = IIRCoefficients::makeHighPass (sampleRate, freq, q);

        filter.setCoefficients (c);
        return filter.processSingleSampleRaw (float (v));
    }

    void reset() override
    {
        filter.reset();
    }

    juce::IIRFilter filter;
};

//==============================================================================
struct HP24State : public FuncState
{
    HP24State (double sr) : FuncState (sr) {}
    
    double process (double v, double freq, double q)
    {
        freq = jlimit (8.0, jmin (20000.0, sampleRate / 2.0), freq);
        q = jmax (0.0000001, q);

        auto c1 = IIRCoefficients::makeHighPass (sampleRate, freq, q);
        auto c2 = IIRCoefficients::makeHighPass (sampleRate, freq, 0.70710678118655f);

        filter1.setCoefficients (c1);
        filter2.setCoefficients (c2);
        return filter2.processSingleSampleRaw (filter1.processSingleSampleRaw (float (v)));
    }
    
    void reset() override
    {
        filter1.reset();
        filter2.reset();
    }
    
    juce::IIRFilter filter1, filter2;
};

//==============================================================================
struct BP12State : public FuncState
{
    BP12State (double sr) : FuncState (sr) {}

    double process (double v, double freq, double q)
    {
        freq = jlimit (8.0, jmin (20000.0, sampleRate / 2.0), freq);
        q = jmax (0.0000001, q);

        auto c = IIRCoefficients::makeBandPass (sampleRate, freq, q);

        filter.setCoefficients (c);
        return filter.processSingleSampleRaw (float (v));
    }

    void reset() override
    {
        filter.reset();
    }

    juce::IIRFilter filter;
};

//==============================================================================
struct BP24State : public FuncState
{
    BP24State (double sr) : FuncState (sr) {}
    
    double process (double v, double freq, double q)
    {
        freq = jlimit (8.0, jmin (20000.0, sampleRate / 2.0), freq);
        q = jmax (0.0000001, q);

        auto c1 = IIRCoefficients::makeBandPass (sampleRate, freq, q);
        auto c2 = IIRCoefficients::makeBandPass (sampleRate, freq, 0.70710678118655f);
        
        filter1.setCoefficients (c1);
        filter2.setCoefficients (c2);
        return filter2.processSingleSampleRaw (filter1.processSingleSampleRaw (float (v)));
    }
    
    void reset() override
    {
        filter1.reset();
        filter2.reset();
    }
    
    juce::IIRFilter filter1, filter2;
};

//==============================================================================
struct LP12State : public FuncState
{
    LP12State (double sr) : FuncState (sr) {}

    double process (double v, double freq, double q)
    {
        freq = jlimit (8.0, jmin (20000.0, sampleRate / 2.0), freq);
        q = jmax (0.0000001, q);

        auto c = IIRCoefficients::makeLowPass (sampleRate, freq, q);

        filter.setCoefficients (c);
        return filter.processSingleSampleRaw (float (v));
    }

    void reset() override
    {
        filter.reset();
    }

    juce::IIRFilter filter;
};

//==============================================================================
struct LP24State : public FuncState
{
    LP24State (double sr) : FuncState (sr) {}
    
    double process (double v, double freq, double q)
    {
        freq = jlimit (8.0, jmin (20000.0, sampleRate / 2.0), freq);
        q = jmax (0.0000001, q);

        auto c1 = IIRCoefficients::makeLowPass (sampleRate, freq, q);
        auto c2 = IIRCoefficients::makeLowPass (sampleRate, freq, 0.70710678118655f);
        
        filter1.setCoefficients (c1);
        filter2.setCoefficients (c2);
        return filter2.processSingleSampleRaw (filter1.processSingleSampleRaw (float (v)));
    }
    
    void reset() override
    {
        filter1.reset();
        filter2.reset();
    }
    
    juce::IIRFilter filter1, filter2;
};

//==============================================================================
struct Notch12State : public FuncState
{
    Notch12State (double sr) : FuncState (sr) {}

    double process (double v, double freq, double q)
    {
        freq = jlimit (8.0, jmin (20000.0, sampleRate / 2.0), freq);
        q = jmax (0.0000001, q);

        auto c = IIRCoefficients::makeNotchFilter (sampleRate, freq, q);

        filter.setCoefficients (c);
        return filter.processSingleSampleRaw (float (v));
    }

    void reset() override
    {
        filter.reset();
    }

    juce::IIRFilter filter;
};

//==============================================================================
struct Notch24State : public FuncState
{
    Notch24State (double sr) : FuncState (sr) {}
    
    double process (double v, double freq, double q)
    {
        freq = jlimit (8.0, jmin (20000.0, sampleRate / 2.0), freq);
        q = jmax (0.0000001, q);

        auto c1 = IIRCoefficients::makeNotchFilter (sampleRate, freq, q);
        auto c2 = IIRCoefficients::makeNotchFilter (sampleRate, freq, 0.70710678118655f);
        
        filter1.setCoefficients (c1);
        filter2.setCoefficients (c2);
        return filter2.processSingleSampleRaw (filter1.processSingleSampleRaw (float (v)));
    }
    
    void reset() override
    {
        filter1.reset();
        filter2.reset();
    }
    
    juce::IIRFilter filter1, filter2;
};

//==============================================================================
class AudioFunctionHost
{
public:
    // You must provide a set of lookup tables at the correct sample rate
    // if you add the oscillator functions
    gin::BandLimitedLookupTables* lookupTables = nullptr;

    void setSampleRate (double sr);
    
    void addConstants (gin::EquationParser&);
    void addUtilities (gin::EquationParser&);
    void addOscillatorFunctions (gin::EquationParser&);
    void addSynthFilterFunctions (gin::EquationParser&);
    void addEffectFilterFunctions (gin::EquationParser&);

protected:
    std::map<int, std::unique_ptr<FuncState>> funcStates;
    double sampleRate = 44100.0;

    template <class T>
    T* getFuncParams (int i, double sr)
    {
        auto itr = funcStates.find (i);
        if (itr != funcStates.end())
        {
            auto p = dynamic_cast<T*> (itr->second.get());
            jassert (p != nullptr);
            return p;
        }

        auto p = new T (sr);
        funcStates[i].reset (p);
        return p;
    }
};
