/*
 *  WebServer.cpp
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>

#include <Factory.h>
#include <WebServer.h>

namespace TWS {
    void WebServer::spawnRequestHandlerThread() {
        pthread_t request_handler_thread;
        pthread_t spam_handler_thread;

        pthread_create(&request_handler_thread, NULL,runHandler, NULL);
        pthread_create(&spam_handler_thread, NULL,runSpamHandler, NULL);
    }

    void *runHandler(void *nothing) {
        // init our socket
        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        struct sockaddr_in si_self;

        // init
        memset((char *)&si_self, 0, sizeof(si_self));
        si_self.sin_family = AF_INET;
        si_self.sin_port = htons(TWS_WEBSERVER_PORT);
        si_self.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(sock, (sockaddr *)&si_self, sizeof(si_self));

        // store incoming data here
        packet_t packet;

        // fix me soon. we need some sort of stop condition
        while (1) {
            recv(sock, &packet, sizeof(packet_t), 0);
            parse_packet(&packet);
        }

        return NULL;
    }

    void *runSpamHandler(void *nothing) {
        // init our socket
        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        struct sockaddr_in si_self;

        // init
        memset((char *)&si_self, 0, sizeof(si_self));
        si_self.sin_family = AF_INET;
        si_self.sin_port = htons(TWS_WEBSERVER_SPAM_PORT);
        si_self.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(sock, (sockaddr *)&si_self, sizeof(si_self));

        // store incoming data here
        // alloc a 1MB buffer
        uint8_t *buff = (uint8_t *)malloc(1024*1024);

        // fix me soon. we need some sort of stop condition
        while (1) {
            size_t len = recv(sock, buff, 1024*1024, 0);
            //std::cerr << "GOT " << len << " bytes" << std::endl;
            //set_spam_nodes(buff);
            uint32_t node_count = len / sizeof(packet_t);
            packet_t *packet = (packet_t *)buff;
            for (uint32_t i=0; i<node_count; i++) {
                //std::cerr << "HIT COUNT: " << packet[i].type << " lat: " << packet[i].latitude << " lon: " << packet[i].longitude << std::endl;
                set_spam_nodes(packet, node_count);
            }
        }

        return NULL;
    }

    void parse_packet(packet_t *packet) {
        Camera *cam = TWS::getCamera();
        double distance = (cam->position().length() - TWS::getClosestPlanet()->_radius) * TWS::getDt() * 10.0;
        float rot = 90.0 * TWS::getDt();
        const float vec[] = {packet->latitude, packet->longitude, 30000.0};
        Vector3d newpos = TWS::getClosestPlanet()->geocentricToCarthesian(vec[0], vec[1], vec[2]);

        switch (packet->type) {
            case CMD_UP : cam->moveupward(distance, false); break;
            case CMD_DOWN: cam->moveupward(-distance, false); break;
            case CMD_LEFT: cam->straferight(-distance, false); break;
            case CMD_RIGHT: cam->straferight(distance, false); break;
            case CMD_STOP: cam->stop(); break;
            case CMD_ZOOMIN: cam->moveforward(-distance, false); break;
            case CMD_ZOOMOUT: cam->moveforward(distance, false); break;
            case CMD_SPACESTATION: cam->follownode(TWS::getISS()); break;
            case CMD_GOTO: cam->setPosition(newpos); break;
            case CMD_ROTATELEFT: cam->rotatey(rot); break;
            case CMD_ROTATERIGHT: cam->rotatey(-rot); break;
            case CMD_ROTATEUP: cam->rotatex(rot); break;
            case CMD_ROTATEDOWN: cam->rotatex(-rot); break;
            case CMD_ROTATEFRONT: cam->rotatez(-rot); break;
            case CMD_ROTATEBACK: cam->rotatez(rot); break;
        }

        std::cerr << "Got a packet of type: " << packet->type << std::endl;
    }
}
