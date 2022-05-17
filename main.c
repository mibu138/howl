#include <portaudio.h>
#include <stdio.h>
#include <string.h>
#define HELL_SIMPLE_NAMES
#define HELL_SIMPLE_FUNCTION_NAMES
#include <hell/hell.h>
#include <hell/minmax.h>
#include <math.h>

#define CE(err)                                                                \
    if (err)                                                                   \
    {                                                                          \
        Print("PA_ERROR: %d\n", err);                                          \
        hell_Exit(1);                                                          \
    }

#define SAMPLE_RATE 44100

typedef struct SinWave {
    float freq;
    float amp;
    float phase;
} SinWave;

typedef struct WaveData {
    SinWave sinwaves[5];
    int     sinwavecount;
    float   t;
} WaveData;

float
sample_sin_wave(SinWave* w, float t)
{
    float s = sin(2 * M_PI * w->freq * t + w->phase) * w->amp;
    return s;
}

int
callback(const void* inbuf, void* outbuf, unsigned long framecount,
         const PaStreamCallbackTimeInfo* timeinfo, PaStreamCallbackFlags status,
         void* userdata)
{
    WaveData* wd  = userdata;
    SinWave*  sw  = &wd->sinwaves[0];
    float*    out = outbuf;
    float     t   = wd->t;

    for (int i = 0; i < framecount; i++)
    {
        out[i] = sample_sin_wave(sw, t);
        t += 1.0 / SAMPLE_RATE;
    }
    wd->t = t;
    return 0;
}

WaveData
makewave1(void)
{
    WaveData w;

    memset(&w, 0, sizeof(w));
    w.sinwavecount      = 1;
    w.sinwaves[0].freq  = 420.0;
    w.sinwaves[0].phase = 0.0;
    w.sinwaves[0].amp   = 0.25;
    return w;
}

Hellmouth hm;
PaStream* stream;

void
shutdown(void)
{
    PaError err;
    err = Pa_StopStream(stream);
    CE(err);
    err = Pa_CloseStream(stream);
    CE(err);
    err = Pa_Terminate();
    CE(err);
    hell_CloseAndExit(&hm);
}

WaveData wd;

void
frame(i64 fi, i64 dt)
{
    int          evc;
    const Event* events = hell_GetEvents(&hm, &evc);
    for (int i = 0; i < evc; i++)
    {
        const Event* ev = &events[i];
        if (ev->type == HELL_EVENT_TYPE_KEYDOWN)
        {
            float amp = wd.sinwaves[0].amp;
            switch (hell_GetEventKeyCode(ev))
            {
            case HELL_KEY_A:
                wd.sinwaves[0].freq -= 20.0;
                break;
            case HELL_KEY_S:
                wd.sinwaves[0].freq += 20.0;
                break;
            case HELL_KEY_J:
                wd.sinwaves[0].amp = fmax(amp - 0.01, 0.0);
                break;
            case HELL_KEY_K:
                wd.sinwaves[0].amp = fmin(amp + 0.01, 1.0);
                break;
            case HELL_KEY_Q:
                shutdown();
            }
        }
    }
}

int
main(int argc, char* argv[])
{
    wd = makewave1();
    PaError err;
    wd.sinwaves[0].amp = 0.0;

    printf("Sup foo\n");
    err = Pa_Initialize();
    CE(err);
    err = Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, SAMPLE_RATE, 256,
                               callback, &wd);
    CE(err);
    err = Pa_StartStream(stream);

    OpenHellmouthNoConsole(frame, shutdown, &hm);
    hell_HellmouthAddWindow(&hm, 500, 500, NULL);

    Loop(&hm);
    return 0;
}
