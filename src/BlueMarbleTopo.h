/*
 *  BlueMarbleTopo.h
 *
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#ifndef _TWS_BLUEMARBLETOPO
#define _TWS_BLUEMARBLETOPO

#include <stdint.h>

namespace TWS {
    class Planet;

    class BlueMarbleTopo {
        public:
            static uint8_t *readTopography(Planet *planet, size_t cubeface, size_t quadtreeindex, size_t lodLevel, uint8_t *heightMap);
            static uint8_t *readTextures(Planet *planet, size_t cubeface, size_t quadtreeindex, size_t lodLevel, uint8_t *textureMap);
            static uint8_t *readNormals(Planet *planet, size_t cubeface, size_t quadtreeindex, size_t lodLevel, uint8_t *normalMap);
            static uint8_t *resizeImage(uint8_t *orig, int width, int height, int channels, uint8_t* resampled, int resampled_width, int resampled_height);
    };
};

#endif
