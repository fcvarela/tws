/*
 *  TerrainManager.h
 *  
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_TERRAINLOADER
#define _TWS_TERRAINLOADER

#include <sys/time.h>
#include <list>
#include <pthread.h>

#include <BlueMarbleTopo.h>
#include <Planet.h>

namespace TWS {
    class TerrainManager {
    public:
        static std::list<TerrainNode *> _nodeQueue;
        static pthread_mutex_t _mutex;
        static pthread_t _thread[4];

        TerrainManager();
        ~TerrainManager();

        static void *spawnThread(void *mutex);
        void requestLoad(TerrainNode *node);
    };
};

#endif
