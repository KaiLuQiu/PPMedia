//
//  ViewController.m
//  PPMedia
//
//  Created by 邱开禄 on 2019/11/14.
//  Copyright © 2019年 邱开禄. All rights reserved.
//

#import "PPMediaMidlle.h"
#include "PPMedia.h"
#include "PPMedia_C_Interface.h"
#include "EventHandler.h"
#include "Message.h"
@interface PPMediaMidlle ()
    @property media::EventHandler *pHandler;
    @property media::msg_loop pMsgLoopCallback;
    -(int) media_player_msg_loop:(media::Message &)msg;
@end

@implementation PPMediaMidlle

-(id) initPlayer:(const char*)URL {
    if(!(self = [super init])) {
        return nil;
    }
    return self;
}

-(void) prepareAsync {
}

-(void) setView:(void *)view
{

}

-(void) start {

}

-(void) pause:(Boolean)isPause {

}

-(void) stop {
    
}

-(void) seek:(float)pos {
}


-(int64_t)getCurPos {
    return 0;
}

-(void)setSpeed {
    
}

-(float)getSpeed {
    return 0.0;
}

-(int64_t)getDuration {
    return 0;
}

-(void)setVolume:(float)value {
//    media::PPMedia::getInstance()->setVolume(value);
}

-(void)dealloc {
    SAFE_DELETE(self.pHandler);
}


void msg_loop(void* playerInstance, media::Message & msg)
{
    // 通过将self指针桥接为oc 对象来调用oc方法
    [(__bridge id)playerInstance media_player_msg_loop:msg];
}

-(int) media_player_msg_loop:(media::Message &)msg
{
//    media::PPMedia::getInstance()->pp_get_msg(msg);
//    switch(msg.m_what)
//    {
//
//        default:
//            break;
//    }
    return 0;
}

-(void)setOnPreparedListener:(id<OnPreparedListener>)listener {
    self.pPreparedListener = listener;
}

-(void)setOnCompletionListener:(id<OnCompletionListener>)listener {
    self.pCompletionListener = listener;
}

-(void)setOnSeekCompletionListener:(id<OnSeekCompletionListener>)listener {
    self.pSeekCompletionListener = listener;
}

-(void)setOnErrorListener:(id<OnErrorListener>)listener {
    self.pErrorListener = listener;
}

-(void)setOnInfoListener:(id<OnInfoListener>)listener {
    self.pInfoListener = listener;
}


@end
