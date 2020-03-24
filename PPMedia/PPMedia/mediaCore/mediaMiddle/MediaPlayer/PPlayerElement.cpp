//
//  PPlayerElement.cpp
//  PPlayerElement
//
//  Created by 邱开禄 on 2020/03/24.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#include "PPlayerElement.h"
NS_MEDIA_BEGIN

PPlayerElement::PPlayerElement()
{
    
}

PPlayerElement::~PPlayerElement()
{

}

// 这边暂时只保留url信息
void PPlayerElement::setDataSource(std::string url)
{
    pUrl = url;
}

bool PPlayerElement::prepareAsync()
{
   return false;
}

void PPlayerElement::prepare()
{
    
}

bool PPlayerElement::start()
{
    return false;

}

bool PPlayerElement::pause(bool state)
{
   return false;

}

int PPlayerElement::seek(float pos)
{
    return -1;
}

bool PPlayerElement::resume()
{
    return false;
}

bool PPlayerElement::stop()
{
    return false;
}

void PPlayerElement::flush()
{
    
}

bool PPlayerElement::setLoop(bool loop)
{
    return false;
}

int64_t PPlayerElement::getCurPos()
{
    return -1;
}

bool PPlayerElement::setSpeed()
{
    return false;
}

float PPlayerElement::getSpeed()
{
    return 0.0;
}

long PPlayerElement::getDuration()
{
    return 0;
}

void PPlayerElement::setVolume(float value)
{
  
}

void PPlayerElement::pp_get_msg(Message& msg)
{
    switch(msg.m_what) {
    }
}
NS_MEDIA_END
