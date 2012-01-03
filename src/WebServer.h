/*
 *  WebServer.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_WEBSERVER
#define _TWS_WEBSERVER

#include <stdint.h>

/*
 * Not really an httpd. Just read command strings from our fifo
 * The command stream is written by the httpd
 */

// the pipe filesystem path
#define TWS_WEBSERVER_PORT 6666
#define TWS_WEBSERVER_SPAM_PORT 6667

namespace TWS
{
#define CMD_UP 1
#define CMD_DOWN 2
#define CMD_LEFT 3
#define CMD_RIGHT 4
#define CMD_STOP 5
#define CMD_ZOOMIN 6
#define CMD_ZOOMOUT 7
#define CMD_SPACESTATION 8
#define CMD_GOTO 9
#define CMD_ROTATELEFT 10
#define CMD_ROTATERIGHT 11
#define CMD_ROTATEUP 12
#define CMD_ROTATEDOWN 13
#define CMD_ROTATEFRONT 14
#define CMD_ROTATEBACK 15

    typedef struct {
        uint32_t type;
        float latitude;
        float longitude;
    } packet_t;

    class WebServer {
        public:
            static void spawnRequestHandlerThread();
    };

    void *runHandler(void *nothing);
    void *runSpamHandler(void *nothing);
    void parse_packet(packet_t *packet);
};

#endif
