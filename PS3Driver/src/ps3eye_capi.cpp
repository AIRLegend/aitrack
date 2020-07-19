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


#include "ps3eye_capi.h"
#include "ps3eye.h"

#include <list>

struct ps3eye_context_t {
    ps3eye_context_t()
        : devices(ps3eye::PS3EYECam::getDevices())
        , opened_devices()
    {
    }

    ~ps3eye_context_t()
    {
    }

    // Global context
    std::vector<ps3eye::PS3EYECam::PS3EYERef> devices;
    std::list<ps3eye_t *> opened_devices;
};

static ps3eye_context_t *
ps3eye_context = NULL;

struct ps3eye_t {
    ps3eye_t(ps3eye::PS3EYECam::PS3EYERef eye, int width, int height, int fps, ps3eye_format outputFormat)
        : eye(eye)
    {
        eye->init(width, height, (uint8_t)fps, (ps3eye::PS3EYECam::EOutputFormat)outputFormat);
        eye->start();
        ps3eye_context->opened_devices.push_back(this);
    }

    ~ps3eye_t()
    {
        eye->stop();
        ps3eye_context->opened_devices.remove(this);
    }

    // Per-device context
    ps3eye::PS3EYECam::PS3EYERef eye;
};

void
ps3eye_init()
{
    if (!ps3eye_context) {
        ps3eye_context = new ps3eye_context_t();
    }
}

void
ps3eye_uninit()
{
    if (ps3eye_context) {
        if (ps3eye_context->opened_devices.size() == 0) {
            delete ps3eye_context;
            ps3eye_context = NULL;
        } else {
            // ERROR: Cannot uninit, there are still handles open
        }
    }
}

int
ps3eye_count_connected()
{
    if (!ps3eye_context) {
        // Not init'ed
        return 0;
    }

    return (int)ps3eye_context->devices.size();
}

ps3eye_t *
ps3eye_open(int id, int width, int height, int fps, ps3eye_format outputFormat)
{
    if (!ps3eye_context) {
        // Library not initialized
        return NULL;
    }

    if (id < 0 || id >= ps3eye_count_connected()) {
        // No such device
        return NULL;
    }

    return new ps3eye_t(ps3eye_context->devices[id], width, height, fps, outputFormat);
}

int
ps3eye_get_unique_identifier(ps3eye_t * eye_t, char *out_identifier, int max_identifier_length)
{
    if (!ps3eye_context) 
    {
        // No context available
        return -1;
    }
    
    if (!eye_t) 
    {
        // Eye is not a valid handle
        return -1;
    }

    bool success = eye_t->eye->getUSBPortPath(out_identifier, max_identifier_length);

    return success ? 0 : -1;
}

void
ps3eye_grab_frame(ps3eye_t *eye, unsigned char* frame)
{
    if (!ps3eye_context) {
        // No context available
        return;
    }

    if (!eye) {
        // Eye is not a valid handle
        return;
    }

	eye->eye->getFrame(frame);
}

void
ps3eye_close(ps3eye_t *eye)
{
    delete eye;
}

int
ps3eye_get_parameter(ps3eye_t *eye, ps3eye_parameter param)
{
    if (!eye) {
        return -1;
    }
    switch (param) {
    case PS3EYE_AUTO_GAIN:
        return eye->eye->getAutogain();
    case PS3EYE_GAIN:
        return eye->eye->getGain();
    case PS3EYE_AUTO_WHITEBALANCE:
        return eye->eye->getAutoWhiteBalance();
    case PS3EYE_EXPOSURE:
        return eye->eye->getExposure();
    case PS3EYE_SHARPNESS:
        return eye->eye->getSharpness();
    case PS3EYE_CONTRAST:
        return eye->eye->getContrast();
    case PS3EYE_BRIGHTNESS:
        return eye->eye->getBrightness();
    case PS3EYE_HUE:
        return eye->eye->getHue();
    case PS3EYE_REDBALANCE:
        return eye->eye->getRedBalance();
    case PS3EYE_BLUEBALANCE:
        return eye->eye->getBlueBalance();
    case PS3EYE_GREENBALANCE:
        return eye->eye->getGreenBalance();
    case PS3EYE_HFLIP:
        return eye->eye->getFlipH();
    case PS3EYE_VFLIP:
        return eye->eye->getFlipV();
    default:
        return -1;
    }
}

int
ps3eye_set_parameter(ps3eye_t *eye, ps3eye_parameter param, int value)
{
    if (!eye) {
        return -1;
    }

    switch (param) {
        case PS3EYE_AUTO_GAIN:
            eye->eye->setAutogain(value > 0);
            break;
        case PS3EYE_GAIN:
            eye->eye->setGain((uint8_t)value);
            break;
        case PS3EYE_AUTO_WHITEBALANCE:
            eye->eye->setAutoWhiteBalance(value > 0);
            break;
        case PS3EYE_EXPOSURE:
			eye->eye->setExposure((uint8_t)value);
            break;
        case PS3EYE_SHARPNESS:
			eye->eye->setSharpness((uint8_t)value);
            break;
        case PS3EYE_CONTRAST:
			eye->eye->setContrast((uint8_t)value);
            break;
        case PS3EYE_BRIGHTNESS:
			eye->eye->setBrightness((uint8_t)value);
            break;
        case PS3EYE_HUE:
			eye->eye->setHue((uint8_t)value);
            break;
        case PS3EYE_REDBALANCE:
			eye->eye->setRedBalance((uint8_t)value);
            break;
        case PS3EYE_BLUEBALANCE:
			eye->eye->setBlueBalance((uint8_t)value);
            break;
        case PS3EYE_GREENBALANCE:
			eye->eye->setGreenBalance((uint8_t)value);
            break;
        case PS3EYE_HFLIP:
            eye->eye->setFlip(value > 0, eye->eye->getFlipV());
            break;
        case PS3EYE_VFLIP:
            eye->eye->setFlip(eye->eye->getFlipH(), value > 0);
            break;
        default:
            break;
    }

    return 0;
}