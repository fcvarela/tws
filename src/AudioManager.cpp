/*
 *  Camera.cpp
 *  
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <Factory.h>
#include <AudioManager.h>

namespace TWS {
    AudioManager::AudioManager() {
        // Audio stuff for tests
        _device = alcOpenDevice(NULL);
        _context = alcCreateContext(_device, NULL);
        alcMakeContextCurrent(_context);
    }

    AudioManager::~AudioManager() {}

    void AudioManager::reposition() {
        TWS::Node *cameraNode = (TWS::Node *)TWS::getCamera();
        Vector3d camPos = cameraNode->position();
        alListener3f(AL_POSITION, camPos.x(), camPos.y(), camPos.z());
    }
}
