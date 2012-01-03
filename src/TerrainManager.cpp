/*
 * TerrainManager.cpp
 *
 *
 * Created by Filipe Varela on 08/10/13.
 * Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <TerrainManager.h>

namespace TWS {
    pthread_mutex_t TerrainManager::_mutex;
    pthread_t TerrainManager::_thread[4];
    std::list<TerrainNode *> TerrainManager::_nodeQueue;

    TerrainManager::TerrainManager() {
        // init mutex
        pthread_mutex_init(&TerrainManager::_mutex, NULL);

        // spawn loader thread
        pthread_create(&_thread[0], NULL, TerrainManager::spawnThread, (void *)NULL);
        //pthread_create(&_thread[1], NULL, TerrainManager::spawnThread, (void *)NULL);
        //pthread_create(&_thread[2], NULL, TerrainManager::spawnThread, (void *)NULL);
        //pthread_create(&_thread[3], NULL, TerrainManager::spawnThread, (void *)NULL);
    }

    TerrainManager::~TerrainManager() {}

    void *TerrainManager::spawnThread(void *_NULL) {
        while (1) {
            // check if list has items
            pthread_mutex_lock(&_mutex);
            size_t count = _nodeQueue.size();
            if (count > 0) {
                // sort them by distance to make sure closer nodes load first
                //_nodeQueue.sort(TerrainNode::compareByDistance);

                // load
                std::list<TerrainNode *>::iterator i = _nodeQueue.begin();
                TerrainNode *node = *i;
                _nodeQueue.pop_front();

                // release ASAP
                pthread_mutex_unlock(&_mutex);
                node->asynch_init();
            } else {
                pthread_mutex_unlock(&_mutex);
                usleep(1000);
            }
        }

        return NULL;
    }

    void TerrainManager::requestLoad(TerrainNode *node) {
        // add node to list
        pthread_mutex_lock(&_mutex);
        _nodeQueue.push_back(node);
        pthread_mutex_unlock(&_mutex);
    }
}
