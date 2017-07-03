//
//  AudioManager.cpp
//  soundTest
//
//  Created by 小松亮太 on 2017/07/03.
//
//

#include "AudioManager.hpp"

#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <AudioToolbox/AudioToolbox.h>
#include <limits.h>
#include <thread>
#include <cmath>

USING_NS_CC;

Scene* SoundTest::createScene()
{
    auto scene = Scene::create();
    auto layer = SoundTest::create();
    scene->addChild(layer);
    return scene;
}

bool SoundTest::init()
{
    if( !Layer::init() ){ return false; }

    //TODO: OSX,IOSの場合は録音する時は初期化の必要がないが録音したデータを鳴らす場合は必要
    ALCdevice* device = alcOpenDevice(nullptr);
    
    //--------音声入力----------//
    const int SAMPLING_RATE = 44100;
    const int SAMPLE_BUFFER_SIZE = 1024;
    
    ALCdevice* capture_device = alcCaptureOpenDevice(nullptr,
                                                     SAMPLING_RATE,
                                                     AL_FORMAT_MONO16,
                                                     SAMPLE_BUFFER_SIZE);
    
    //録音開始
    alcCaptureStart(capture_device);
    log("録音開始\n");

    std::vector<ALshort> buffer;
    size_t buffer_top = 0;
    
    while( buffer_top < SAMPLING_RATE * 2 * 3 )
    {
        //録音されたデータ量を調べる
        ALint sample;
        alcGetIntegerv(capture_device,
                       ALC_CAPTURE_SAMPLES,
                       sizeof(sample),
                       &sample);
        
        if(sample > 0)
        {
            //リサイズ
            buffer.resize( buffer_top + sample );
            
            alcCaptureSamples(capture_device,
                              (ALCvoid*)&buffer[buffer_top], sample);
            buffer_top += sample;
        }
    }
    
    //録音停止
    alcCaptureStop(capture_device);
    alcCaptureCloseDevice(capture_device);

    log("録音完了\n");
    
    //録音したデータを再生
    ALCcontext* context = alcCreateContext(device, nullptr);
    alcMakeContextCurrent(context);
    
    ALuint buffer_id;
    alGenBuffers(1, &buffer_id);
    
    alBufferData( buffer_id, AL_FORMAT_MONO16,&buffer[0], buffer.size()*2, SAMPLING_RATE );
    
    ALuint source_id;
    alGenSources(1, &source_id);
    
    //ソースに再生したいバッファを割り当てる
    alSourcei( source_id, AL_BUFFER, buffer_id );
    
    //再生開始
    alSourcePlay(source_id);
    log("再生開始\n");
    
    std::this_thread::sleep_for( std::chrono::seconds(3) );
    
    auto label = Label::createWithSystemFont("再生終了", "Arial", 80);
    label->setPosition(Director::getInstance()->getVisibleSize()/2);
    this->addChild(label);
    log("再生終了\n");
    
    alDeleteSources(1, &source_id);
    alDeleteBuffers(1, &buffer_id);
    
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
    
    return true;
}
