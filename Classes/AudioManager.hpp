//
//  AudioManager.hpp
//  soundTest
//
//  Created by 小松亮太 on 2017/07/03.
//
//

#ifndef AudioManager_hpp
#define AudioManager_hpp

#include <cocos2d.h>


class SoundTest : public cocos2d::Layer
{
public:
  
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(SoundTest);

};

#endif /* AudioManager_hpp */
