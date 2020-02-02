//
//  PPMedia_C_Interface
//  PPMedia
//
//  Created by 邱开禄 on 2019/12/15.
//  Copyright © 2019年 邱开禄. All rights reserved.
//

#ifndef PPMedia_C_Interface_H
#define PPMedia_C_Interface_H
#include "Message.h"
NS_MEDIA_BEGIN
typedef void (*msg_loop) (void* playerInstance, Message &msg);
NS_MEDIA_END
#endif
