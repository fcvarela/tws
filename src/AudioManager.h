/*
 *  AudioManager.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_AUDIOMANAGER
#define _TWS_AUDIOMANAGER

namespace TWS
{
    class AudioManager {
        private:
            ALCdevice *_device;
            ALCcontext *_context;

        public:
            AudioManager();
            ~AudioManager();
            void reposition();
    };
};

#endif
