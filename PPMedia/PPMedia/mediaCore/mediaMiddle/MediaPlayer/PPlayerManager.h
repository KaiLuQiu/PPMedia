//
//  PPlayerManager.h
//  PPlayerManager
//
//  Created by 邱开禄 on 2020/03/24.
//  Copyright © 2019 邱开禄. All rights reserved.
//

#ifndef PPlayerManager_H
#define PPlayerManager_H
#include <string>
#include "MediaCommon.h"
#include "EventHandler.h"
#include "PPlayerElement.h"

NS_MEDIA_BEGIN
// 定义多路播放的数量
#define MAX_PLAYER_NUMBER 6

/* 设计思想：PPlayerManager 多路播放器的管理者，记录当前正在播放的player器数量
*  空闲播放器数量，以及调度或者删除播放器的管理者。
*  状态：待实现
*/
class PPlayerManager
{
public:
  
    PPlayerManager();

    virtual ~PPlayerManager();
private:
};

NS_MEDIA_END
#endif // PPlayerManager_H
