/*
 *  Sun.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_SUN
#define _TWS_SUN

#include <AudioNode.h>
#include <Vector3.h>

namespace TWS {
    class Sun {
        private:
            AudioNode *_audioNode;
            GLuint m_sphereList;

        public:
            Vector3d _position3d;
            GLfloat _position[3];
            double _radius;

            Sun();
            ~Sun();

            void draw();
            void reposition();
    };
};

#endif
