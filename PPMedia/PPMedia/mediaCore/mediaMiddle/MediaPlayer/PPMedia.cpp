//
//  PPMedia.cpp
//  PPMedia
//
//  Created by 邱开禄 on 2019/11/14.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#include "PPMedia.h"
NS_MEDIA_BEGIN

PPMedia::PPMedia()
{
    
}

PPMedia::~PPMedia()
{

}

void PPMedia::setHandle(EventHandler *handle)
{
}

// 这边暂时只保留url信息
void PPMedia::setDataSource(std::string url)
{
    pUrl = url;
}

int PPMedia::setView(void *view)
{
    return 0;
}

bool PPMedia::prepareAsync()
{
   return false;
}

void PPMedia::prepare()
{
    
}

bool PPMedia::start()
{
    return false;

}

bool PPMedia::pause(bool state)
{
   return false;

}

int PPMedia::seek(float pos)
{
    return -1;
}

bool PPMedia::resume()
{
    return false;
}

bool PPMedia::stop()
{
    return false;
}

void PPMedia::flush()
{
    
}

bool PPMedia::setLoop(bool loop)
{
    return false;
}

int64_t PPMedia::getCurPos()
{
    return -1;
}

bool PPMedia::setSpeed()
{
    return false;
}

float PPMedia::getSpeed()
{
    return 0.0;
}

long PPMedia::getDuration()
{
    return 0;
}

void PPMedia::setVolume(float value)
{
  
}

void PPMedia::pp_get_msg(Message& msg)
{
    switch(msg.m_what) {
    }
}
NS_MEDIA_END
