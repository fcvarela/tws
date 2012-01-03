/*
 *  AudioNode.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_AUDIONODE
#define _TWS_AUDIONODE

#ifdef __APPLE__
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif

#ifdef linux
#include <AL/al.h>
#include <AL/alc.h>
#endif

#include <Vector3.h>

namespace TWS
{
    typedef struct {
        char riff[4]; //'RIFF'
        unsigned int riffSize;
        char wave[4]; //'WAVE'
        char fmt[4]; //'fmt '
        unsigned int fmtSize;
        unsigned short format;
        unsigned short channels;
        unsigned int samplesPerSec;
        unsigned int bytesPerSec;
        unsigned short blockAlign;
        unsigned short bitsPerSample;
        char data[4]; //'data'
        unsigned int dataSize;
    } wave_header_t;

    class AudioNode {
        private:
            ALuint _buffer;
            ALuint _source;
            Vector3d _position;
            Vector3d _velocity;

            void checkALErrors(const char *errLocation);

        public:
            AudioNode(const char *filename);
            ~AudioNode();

            void play();
            void stop();

            void setPosition(Vector3d newPosition);
    };
};

#endif
