/*
 *  Factory.h
 *  TWS
 *
 *  Created by Filipe Varela on 08/11/05.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_FACTORY
#define _TWS_FACTORY

#include <stdint.h>
#include <pthread.h>
#include <string>
#include <GL.h>
#include <Camera.h>
#include <Planet.h>
#include <Sun.h>
#include <Console.h>
#include <AudioManager.h>
#include <Node.h>
#include <WebServer.h>
#include <Config.h>
#include <TerrainManager.h>

namespace TWS {
    #define min(a,b) (a < b) ? a : b
    #define max(a,b) (a > b) ? a : b

    Camera *getCamera(void);
    Planet *getClosestPlanet(void);
    Planet *getEarth(void);
    Planet *getMars(void);
    std::string getConfig(std::string const& section, std::string const& entry);
    Sun *getSun(void);
    Node *getISS(void);
    TerrainManager *getTerrainManager(void);
    Console *getConsole();

    void setConfigFile(std::string const& filename);

    extern packet_t *spam_nodes;
    extern uint32_t spam_node_count;

    AudioManager *getAudioManager(void);
    pthread_mutex_t *getLoaderMutex(void);
    void setTime(double a_time);
    double getDt(void);
    void set_spam_nodes(packet_t *nodes, uint32_t node_count);

    extern float fps;
    extern size_t drawn_nodes;
    extern double hfov;
    extern double vfov;
    extern double hsiz;
    extern double vsiz;
    extern double dt;
    extern double dtime;
    extern double fardist;
}

#endif
