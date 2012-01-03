/*
 *  Camera.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_CAMERA
#define _TWS_CAMERA

#include <Node.h>

namespace TWS
{
    class Camera : public Node {
        public:
            Camera();
            ~Camera();

            void setPerspective();
    };
};

#endif
