/**
 * PS3EYEDriver C API Interface
 * Copyright (c) 2014 Thomas Perl <m@thp.io>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/

#ifndef PS3EYEDRIVER_H
#define PS3EYEDRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ps3eye_t ps3eye_t;

typedef enum{
    PS3EYE_AUTO_GAIN,           // [false, true]
    PS3EYE_GAIN,                // [0, 63]
    PS3EYE_AUTO_WHITEBALANCE,   // [false, true]
    PS3EYE_EXPOSURE,            // [0, 255]
    PS3EYE_SHARPNESS,           // [0 63]
    PS3EYE_CONTRAST,            // [0, 255]
    PS3EYE_BRIGHTNESS,          // [0, 255]
    PS3EYE_HUE,                 // [0, 255]
    PS3EYE_REDBALANCE,          // [0, 255]
    PS3EYE_BLUEBALANCE,         // [0, 255]
    PS3EYE_GREENBALANCE,        // [0, 255]
    PS3EYE_HFLIP,               // [false, true]
    PS3EYE_VFLIP                // [false, true]
} ps3eye_parameter;

typedef enum{
	PS3EYE_FORMAT_BAYER,        // Output in Bayer. Destination buffer must be width * height bytes
	PS3EYE_FORMAT_BGR,          // Output in BGR. Destination buffer must be width * height * 3 bytes
	PS3EYE_FORMAT_RGB,          // Output in RGB. Destination buffer must be width * height * 3 bytes
} ps3eye_format;


/**
 * Initialize and enumerate connected cameras.
 * Needs to be called once before all other API functions.
 **/
void
ps3eye_init();

/**
 * De-initialize the library and free resources.
 * If a pseye_t * object is still opened, nothing happens.
 **/
void
ps3eye_uninit();

/**
 * Return the number of PSEye cameras connected via USB.
 **/
int
ps3eye_count_connected();

/**
 * Open a PSEye camera device by id.
 * The id is zero-based, and must be smaller than the count.
 * width and height should usually be 640x480 or 320x240
 * fps is the target frame rate, 60 usually works fine here
 **/
ps3eye_t *
ps3eye_open(int id, int width, int height, int fps, ps3eye_format outputFormat);

/**
 * Get the string that uniquely identifies this camera
 * Returns 0 on success, -1 on failure
 **/
int
ps3eye_get_unique_identifier(ps3eye_t * eye, char *out_identifier, int max_identifier_length);

/**
 * Grab the next frame as YUV422 blob.
 * A pointer to the buffer will be passed back. The buffer
 * will only be valid until the next call, or until the eye
 * is closed again with ps3eye_close(). If stride is not NULL,
 * the byte offset between two consecutive lines in the frame
 * will be written to *stride.
 **/
void
ps3eye_grab_frame(ps3eye_t *eye, unsigned char* frame);

/**
 * Close a PSEye camera device and free allocated resources.
 * To really close the library, you should also call ps3eye_uninit().
 **/
void
ps3eye_close(ps3eye_t *eye);

/**
 * Set a ps3eye_parameter to a value.
 * Returns -1 if there is an error, otherwise 0.
 **/
int
ps3eye_set_parameter(ps3eye_t *eye, ps3eye_parameter param, int value);

/**
* Get a ps3eye_parameter value.
* Returns -1 if there is an error, otherwise returns the parameter value int.
**/
int
ps3eye_get_parameter(ps3eye_t *eye, ps3eye_parameter param);

#ifdef __cplusplus
};
#endif

#endif /* PS3EYEDRIVER_H */
