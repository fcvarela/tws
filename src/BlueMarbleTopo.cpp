/*
 *  BlueMarbleTopo.h
 *  
 *
 *  Created by Filipe Varela on 08/10/13.
 *  Copyright 2008 Filipe Varela. All rights reserved.
 *
 */

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <math.h>
#include <stdint.h>
#include <pthread.h>
#include <string>
#include <GL.h>

#include <Factory.h>
#include <BlueMarbleTopo.h>

namespace TWS {
    uint8_t *BlueMarbleTopo::readTopography(Planet *planet, size_t cubeface, size_t quadtreeindex, size_t lodLevel, uint8_t *normalMap) {
        // figure out exact texture size
        size_t texture_size = min(32*pow(2, lodLevel), 512);
        texture_size+=2;

        // prepare a filename
        char cache_filename[1024];
        sprintf(cache_filename, "%s/topo%lu.%lu.%lu", planet->_cachedir, cubeface, lodLevel, quadtreeindex);

        // check for existing cached file and return cached data
        FILE *cachefile = fopen(cache_filename, "r");
        if (cachefile != NULL) {
            size_t len = fread(normalMap, (texture_size-2)*(texture_size-2)*3, sizeof(uint8_t), cachefile);
            fclose(cachefile);
            len = 0;
            return normalMap;
        }

        uint8_t *_normalBuffer = (uint8_t *)malloc(sizeof(uint8_t)*texture_size*texture_size*2);
        memset(_normalBuffer, 0, sizeof(uint8_t)*texture_size*texture_size*2);

        char filename[1024];
        sprintf(filename, "data/%s/topo%d.ppm", planet->_label, (int)cubeface);
        FILE *normalFile = fopen(filename, "rb");
        if (normalFile == NULL) {
            perror("TWS::BlueMarbleTopo error opening file");
            return NULL;
        }

        // cube face offset
        size_t off_y = 0, off_x = 0;

        // walk quadtree and figure out additional offsetting: right to left
        size_t temp_qt = quadtreeindex;
        size_t digit;
        size_t index = lodLevel;

        while (temp_qt > 0) {
            // position in qt level (0,1,2,3)
            digit = temp_qt % 10;

            // calculate offset for this lod level
            size_t l_step = pow(2, index) * 32;
            if (digit == 2 || digit == 4)
                off_x += l_step;
            if (digit == 3 || digit == 4)
                off_y += l_step;

            temp_qt = temp_qt/10;
            index++;
        }

        // figure out texture step (not vertex step)
        size_t texture_step = 0;
        switch (lodLevel) {
            case 0:
            case 1:
            case 2:
            case 3: texture_step = 1; break;  //   256
            case 4: texture_step = 1; break;  //   512
            case 5: texture_step = 2; break;  //  1024
            case 6: texture_step = 4; break;  //  2048
            case 7: texture_step = 8; break;  //  4096
            case 8: texture_step = 16; break; //  8192
            case 9: texture_step = 32; break; // 16384
        }

        uint8_t *normalBuff = (uint8_t *)malloc(sizeof(uint8_t)*texture_size*(texture_step)*3);

        // samplesperrow
        size_t samples_per_row = 0, file_offset = 0;
        switch (planet->_maxlod) {
            case 8:
                samples_per_row = 8194;
                file_offset = 17;
                break;
            case 9:
                samples_per_row = 16386;
                file_offset=19;
                break;
        }

        // read heightmap
        for (size_t y=0; y<(uint)texture_size; y++) {
            size_t offset = (off_y + y*texture_step)*samples_per_row + off_x;
            fseek(normalFile, file_offset+offset*3L, SEEK_SET);
            size_t len = fread(normalBuff, texture_size*texture_step*3, sizeof(uint8_t), normalFile);
            len = 0;
            for (size_t x=0; x<(uint)texture_size; x++) {
                int16_t val;
                memcpy(&val, &normalBuff[x*texture_step*3+0], sizeof(int16_t));
                uint16_t val2 = val + 32768;
                memcpy(&_normalBuffer[y*texture_size*2+x*2+0], &val2, sizeof(uint16_t));
            }
        }

        free(normalBuff);

        // resize texture to power-of-two
        resizeImage(_normalBuffer, texture_size, texture_size, 2, normalMap, texture_size-2, texture_size-2);

        // cleanup and return
        free(_normalBuffer);
        fclose(normalFile);

        // cache the data
        cachefile = fopen(cache_filename, "w");
        if (cachefile != NULL) {
            fwrite(normalMap, (texture_size-2)*(texture_size-2)*2, sizeof(uint8_t), cachefile);
            fclose(cachefile);
        }

        return normalMap;
    }

    uint8_t *BlueMarbleTopo::readTextures(Planet *planet, size_t cubeface, size_t quadtreeindex, size_t lodLevel, uint8_t *textureMap) {
        size_t samples_per_row = 0, file_offset = 0, bytes_per_sample = 0;
        switch (planet->_maxlod) {
            case 8:
                samples_per_row = 8194;
                file_offset = 17;
                bytes_per_sample = 3;
                break;
            case 9:
                samples_per_row = 16386;
                file_offset=19;
                bytes_per_sample = 4;
                break;
        }

        // figure out exact texture size
        size_t texture_size = min(32*pow(2, lodLevel), 512);
        texture_size+=2;

        // prepare a filename
        char cache_filename[1024];
        sprintf(cache_filename, "%s/texture%lu.%lu.%lu", planet->_cachedir, cubeface, lodLevel, quadtreeindex);

        // check for existing cached file and return cached data
        FILE *cachefile = fopen(cache_filename, "r");
        if (cachefile != NULL) {
            size_t len = fread(textureMap, (texture_size-2)*(texture_size-2)*bytes_per_sample, sizeof(uint8_t), cachefile);
            fclose(cachefile);
            len = 0;
            return textureMap;
        }

        uint8_t *_textureBuffer = (uint8_t *)malloc(sizeof(uint8_t)*texture_size*texture_size*bytes_per_sample);
        memset(_textureBuffer, 0, sizeof(uint8_t)*texture_size*texture_size*bytes_per_sample);

        char filename[1024];
        sprintf(filename, "data/%s/rgbspec%d.ppm", planet->_label, (int)cubeface);
        FILE *textureFile = fopen(filename, "rb");
        if (textureFile == NULL) {
            perror("TWS::BlueMarbleTopo error opening file");
            return NULL;
        }

        // cube face offset
        size_t off_y = 0, off_x = 0;

        // walk quadtree and figure out additional offsetting: right to left
        size_t temp_qt = quadtreeindex;
        size_t digit;
        size_t index = lodLevel;

        while (temp_qt > 0) {
            // position in qt level (0,1,2,3)
            digit = temp_qt % 10;

            // calculate offset for this lod level
            size_t l_step = pow(2, index) * 32;
            if (digit == 2 || digit == 4)
                off_x += l_step;
            if (digit == 3 || digit == 4)
                off_y += l_step;

            temp_qt = temp_qt/10;
            index++;
        }

        // figure out texture step (not vertex step)
        size_t texture_step = 0;
        switch (lodLevel) {
            case 0:
            case 1:
            case 2:
            case 3: texture_step = 1; break;
            case 4: texture_step = 1; break;
            case 5: texture_step = 2; break;
            case 6: texture_step = 4; break;
            case 7: texture_step = 8; break;
            case 8: texture_step = 16; break;
            case 9: texture_step = 32; break;
        }

        uint8_t *textureBuff = (uint8_t *)malloc(sizeof(uint8_t)*texture_size*(texture_step)*bytes_per_sample);

        // read heightmap
        for (size_t y=0; y<(uint)texture_size; y++) {
            size_t offset = (off_y + y*texture_step)*samples_per_row + off_x;
            fseek(textureFile, file_offset+offset*bytes_per_sample, SEEK_SET);
            size_t len = fread(textureBuff, texture_size*texture_step*bytes_per_sample, sizeof(uint8_t), textureFile);
            len = 0;
            for (size_t x=0; x<(uint)texture_size; x++) {
                _textureBuffer[y*texture_size*bytes_per_sample+x*bytes_per_sample+0] = textureBuff[x*texture_step*bytes_per_sample+0];
                _textureBuffer[y*texture_size*bytes_per_sample+x*bytes_per_sample+1] = textureBuff[x*texture_step*bytes_per_sample+1];
                _textureBuffer[y*texture_size*bytes_per_sample+x*bytes_per_sample+2] = textureBuff[x*texture_step*bytes_per_sample+2];
                if (bytes_per_sample == 4)
                    _textureBuffer[y*texture_size*bytes_per_sample+x*bytes_per_sample+3] = textureBuff[x*texture_step*bytes_per_sample+3];
            }
        }

        free(textureBuff);

        // resize texture to power-of-two
        resizeImage(_textureBuffer, texture_size, texture_size, bytes_per_sample, textureMap, texture_size-2, texture_size-2);

        // cleanup and return
        free(_textureBuffer);
        fclose(textureFile);

        // cache the data
        cachefile = fopen(cache_filename, "w");
        if (cachefile != NULL) {
            fwrite(textureMap, (texture_size-2)*(texture_size-2)*bytes_per_sample, sizeof(uint8_t), cachefile);
            fclose(cachefile);
        }

        return textureMap;
    }

    uint8_t *BlueMarbleTopo::readNormals(Planet *planet, size_t cubeface, size_t quadtreeindex, size_t lodLevel, uint8_t *normalMap) {
        // figure out exact texture size
        size_t texture_size = min(32*pow(2, lodLevel), 512);
        texture_size+=2;

        // prepare a filename
        char cache_filename[1024];
        sprintf(cache_filename, "%s/normal%lu.%lu.%lu", planet->_cachedir, cubeface, lodLevel, quadtreeindex);

        // check for existing cached file and return cached data
        FILE *cachefile = fopen(cache_filename, "r");
        if (cachefile != NULL) {
            size_t len = fread(normalMap, (texture_size-2)*(texture_size-2)*3, sizeof(uint8_t), cachefile);
            fclose(cachefile);
            len = 0;
            return normalMap;
        }

        uint8_t *_normalBuffer = (uint8_t *)malloc(sizeof(uint8_t)*texture_size*texture_size*3);
        memset(_normalBuffer, 0, sizeof(uint8_t)*texture_size*texture_size*3);

        char filename[1024];
        sprintf(filename, "data/%s/normals%d.ppm", planet->_label, (int)cubeface);
        FILE *normalFile = fopen(filename, "rb");
        if (normalFile == NULL) {
            perror("TWS::BlueMarbleTopo error opening file");
            return NULL;
        }

        // cube face offset
        size_t off_y = 0, off_x = 0;

        // walk quadtree and figure out additional offsetting: right to left
        size_t temp_qt = quadtreeindex;
        size_t digit;
        size_t index = lodLevel;

        while (temp_qt > 0) {
            // position in qt level (0,1,2,3)
            digit = temp_qt % 10;

            // calculate offset for this lod level
            size_t l_step = pow(2, index) * 32;
            if (digit == 2 || digit == 4)
                off_x += l_step;
            if (digit == 3 || digit == 4)
                off_y += l_step;

            temp_qt = temp_qt/10;
            index++;
        }

        // figure out texture step (not vertex step)
        size_t texture_step = 0;
        switch (lodLevel) {
            case 0:
            case 1:
            case 2:
            case 3: texture_step = 1; break;
            case 4: texture_step = 1; break;
            case 5: texture_step = 2; break;
            case 6: texture_step = 4; break;
            case 7: texture_step = 8; break;
            case 8: texture_step = 16; break;
            case 9: texture_step = 32; break;
        }

        uint8_t *normalBuff = (uint8_t *)malloc(sizeof(uint8_t)*texture_size*(texture_step)*3);
        size_t samples_per_row = 0, file_offset = 0;
        switch (planet->_maxlod) {
            case 8:
                samples_per_row = 8194;
                file_offset = 17;
                break;
            case 9:
                samples_per_row = 16386;
                file_offset=19;
                break;
        }

        // read heightmap
        for (size_t y=0; y<(uint)texture_size; y++) {
            size_t offset = (off_y + y*texture_step)*samples_per_row + off_x;
            fseek(normalFile, file_offset+offset*3L, SEEK_SET);
            size_t len = fread(normalBuff, texture_size*texture_step*3, sizeof(uint8_t), normalFile);
            len = 0;
            for (size_t x=0; x<(uint)texture_size; x++) {
                _normalBuffer[y*texture_size*3+x*3+0] = normalBuff[x*texture_step*3+0];
                _normalBuffer[y*texture_size*3+x*3+1] = normalBuff[x*texture_step*3+1];
                _normalBuffer[y*texture_size*3+x*3+2] = normalBuff[x*texture_step*3+2];
            }
        }

        free(normalBuff);

        // resize texture to power-of-two
        resizeImage(_normalBuffer, texture_size, texture_size, 3, normalMap, texture_size-2, texture_size-2);

        // cleanup and return
        free(_normalBuffer);
        fclose(normalFile);

        // cache the data
        cachefile = fopen(cache_filename, "w");
        if (cachefile != NULL) {
            fwrite(normalMap, (texture_size-2)*(texture_size-2)*3, sizeof(uint8_t), cachefile);
            fclose(cachefile);
        }

        return normalMap;
    }

    uint8_t *BlueMarbleTopo::resizeImage(uint8_t *orig, int width, int height, int channels, uint8_t* resampled, int resampled_width, int resampled_height) {
        if ((width < 1) || (height < 1) || (resampled_width < 2) || (resampled_height < 2) || (channels < 1) || (NULL == orig) || (NULL == resampled))
            return NULL;

        float dx = (width - 1.0f) / (resampled_width - 1.0f);
        float dy = (height - 1.0f) / (resampled_height - 1.0f);

        for (int y=0; y<resampled_height; ++y) {
            float sampley = y * dy;
            int inty = (int)sampley;
            if (inty > height - 2)
                inty = height - 2;
            sampley -= inty;

            for (int x=0; x<resampled_width; ++x) {
                float samplex = x * dx;
                int intx = (int)samplex;
                int base_index;
                // find the base x index and fractional offset from that
                if (intx > width - 2)
                    intx = width - 2;
                samplex -= intx;
                /*	base index into the original image	*/
                base_index = (inty * width + intx) * channels;
                for (int c=0; c<channels; ++c) {
                    /*	do the sampling	*/
                    float value = 0.5f;
                    value += orig[base_index] * (1.0f-samplex) * (1.0f-sampley);
                    value += orig[base_index+channels] * (samplex) * (1.0f-sampley);
                    value += orig[base_index+width*channels] * (1.0f-samplex) * (sampley);
                    value += orig[base_index+width*channels+channels] * (samplex) * (sampley);
                    ++base_index;
                    resampled[y*resampled_width*channels+x*channels+c] = (uint8_t)(value);
                }
            }
        }
        return resampled;
    }
};

