/*
 *  AudioNode.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Factory.h>
#include <AudioNode.h>

namespace TWS {
    AudioNode::AudioNode(const char *filename) {
        // Make sure OpenAL is initialized
        TWS::getAudioManager();

        // Generate buffers
        alGenBuffers(1, &_buffer);
        checkALErrors("alGenBuffers");

        FILE* file = fopen(filename,"rb");
        if (!file) {
            printf("ERROR reading file\n");
        }

        wave_header_t *header = (wave_header_t *)malloc(sizeof(wave_header_t));
        if (!fread(header, sizeof(wave_header_t), 1, file)) {
            printf("ERROR freading file\n");
        }

        if ((memcmp("RIFF", header->riff, 4) || memcmp("WAVE", header->wave, 4) || memcmp("fmt ", header->fmt, 4) || memcmp("data", header->data, 4))) {
            printf("ERROR in format\n");
        }

        char *buffer = (char*)malloc(header->dataSize);
        size_t datasize = fread(buffer, header->dataSize, 1, file);
        datasize = 0;
        fclose(file);

        ALuint format = 0;
        switch (header->bitsPerSample) {
            case 8: format = (header->channels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8; break;
            case 16: format = (header->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16; break;
            default: printf("ERROR in format\n"); break;
        }

        alBufferData(_buffer, format, buffer, header->dataSize, header->samplesPerSec);
        checkALErrors("alBufferData");
        free(buffer);
        free(header);

        // Generate source
        alGenSources(1, &_source);
        checkALErrors("alGenSources");
        alSourcef(_source, AL_PITCH, 1.0f);
        alSourcef(_source, AL_GAIN, 1.0f);
        alSourcei(_source, AL_BUFFER, _buffer);
        alSourcei(_source, AL_LOOPING, AL_TRUE);
    }

    void AudioNode::setPosition(Vector3d newPosition) {
        _position = newPosition;
        alSource3f(_source, AL_POSITION, _position.x(), _position.y(), _position.z());
    }

    void AudioNode::play() {alSourcePlay(_source);}
    void AudioNode::stop() {alSourceStop(_source);}

    AudioNode::~AudioNode() {}

    void AudioNode::checkALErrors(const char *errLocation) {
        ALenum err = alGetError();
        if (err == AL_NO_ERROR)
            return;

        switch(err) {
            case AL_INVALID_NAME: printf("%s: AL_INVALID_NAME\n", errLocation); break;
            case AL_INVALID_ENUM: printf("%s: AL_INVALID_ENUM\n", errLocation); break;
            case AL_INVALID_VALUE: printf("%s: AL_INVALID_VALUE\n", errLocation); break;
            case AL_INVALID_OPERATION: printf("%s: AL_INVALID_OPERATION\n", errLocation); break;
            case AL_OUT_OF_MEMORY: printf("%s: AL_OUT_OF_MEMORY\n", errLocation); break;
            default: printf("%s: UNKNOWN ERROR\n", errLocation); break;
        }

        exit(1);
    }
}
