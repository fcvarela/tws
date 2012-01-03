/*
 *  Factory.cpp
 *  TWS
 *
 *  Created by Filipe Varela on 08/11/05.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#include <Factory.h>
#include <Vector3.h>
#include <ObjModel.h>

namespace TWS {
    static Camera *gCamera;
    static Planet *gEarth;
    static Planet *gMars;
    static Sun *gSun;
    static AudioManager *gAudioManager;
    static Node *gISS;
    static Config *gConfig;
    static TerrainManager *gTerrainManager;
    static Console *gConsole;

    double dt = 0.0;
    double dtime = 0.0;
    double hfov = 0.0;
    double vfov = 0.0;
    double hsiz = 0.0;
    double vsiz = 0.0;
    double fardist = 0.0;
    float fps = 0.0;

    packet_t *spam_nodes = NULL;
    uint32_t spam_node_count = 0;

    size_t drawn_nodes = 0;

    void setTime(double a_time) {
        dt = a_time - dtime;
        fps = 1.0/dt;
        dtime = a_time;
    }

    void set_spam_nodes(packet_t *nodes, uint32_t node_count) {
        spam_nodes = nodes;
        spam_node_count = node_count;
    }

    double getDt(void) {
        return dt;
    }

    Camera *getCamera(void) {
        if (!gCamera)
            gCamera = new Camera();

        return gCamera;
    }

    TerrainManager *getTerrainManager(void) {
        if (!gTerrainManager)
            gTerrainManager = new TerrainManager();

        return gTerrainManager;
    }

    Planet *getClosestPlanet(void) {
        if (!gEarth)
            getEarth();

        if (!gMars)
            getMars();

        Camera *cam = getCamera();
        double dist_earth = (cam->position() - gEarth->_position).length();
        double dist_mars = (cam->position() - gMars->_position).length();

        if (dist_mars < dist_earth) {
            //std::cerr << "CLOSEST PLANET: MARS" << std::endl;
            return gMars;
        }

        //std::cerr << "CLOSEST PLANET: EARTH" << std::endl;
        return gEarth;
    }

    Planet *getEarth(void) {
        if (!gEarth) {
            double radius = strtod(TWS::getConfig("earth", "radius").c_str(), NULL);
            double mass = strtod(TWS::getConfig("earth", "mass").c_str(), NULL);
            size_t maxlod = atol(TWS::getConfig("earth", "maxlod").c_str());
            double x, y, z;
            x = strtod(TWS::getConfig("earth", "position.x").c_str(), NULL);
            y = strtod(TWS::getConfig("earth", "position.y").c_str(), NULL);
            z = strtod(TWS::getConfig("earth", "position.z").c_str(), NULL);
            Vector3d position = Vector3d(x, y, z);
            const char *name = TWS::getConfig("earth", "name").c_str();
            gEarth = new Planet(name, position, radius, mass, maxlod);
        }
        return gEarth;
    }

    Planet *getMars(void) {
        if (!gMars) {
            double radius = strtod(TWS::getConfig("mars", "radius").c_str(), NULL);
            double mass = strtod(TWS::getConfig("mars", "mass").c_str(), NULL);
            size_t maxlod = atol(TWS::getConfig("mars", "maxlod").c_str());
            double x, y, z;
            x = strtod(TWS::getConfig("mars", "position.x").c_str(), NULL);
            y = strtod(TWS::getConfig("mars", "position.y").c_str(), NULL);
            z = strtod(TWS::getConfig("mars", "position.z").c_str(), NULL);
            Vector3d position = Vector3d(x, y, z);
            const char *name = TWS::getConfig("mars", "name").c_str();
            gMars = new Planet(name, position, radius, mass, maxlod);
        }
        return gMars;
    }

    void setConfigFile(std::string const& filename) {
        if (!gConfig) gConfig = new TWS::Config(filename);
    }

    std::string getConfig(std::string const& section, std::string const& entry) {
        if (!gConfig) {std::cerr << "TWS::Config failed to parse config " << section << "." << entry << std::endl; return NULL;}
        return gConfig->Value(section, entry);
    }

    Sun *getSun(void) {
        if (!gSun)
            gSun = new Sun();

        return gSun;
    }

    Node *getISS(void) {
        if (!gISS) {
            gISS = new Node();

            // quick hack: instantiate a node and load/attach a model
            gISS = new Node();
            Model *model = new TWS::ObjModel();
            model->load("data/models/out.obj");
            gISS->setModel(model);

            Vector3d initialVelocity = Vector3d(1.0, 0.0, 0.0) * 7706.6; // avg ISS velocity in m/s
            Vector3d initialPosition = Vector3d(0.0, 0.0, 1.0) * (347000.0 + TWS::getEarth()->_radius); // 347 Km above surface

            gISS->setPosition(initialPosition);
            gISS->setVelocity(initialVelocity);
        }

        return gISS;
    }

    AudioManager *getAudioManager(void) {
        if (!gAudioManager)
            gAudioManager = new AudioManager();

        return gAudioManager;
    }

    Console *getConsole(void) {
        if (!gConsole)
            gConsole = new Console();

        return gConsole;
    }
}
