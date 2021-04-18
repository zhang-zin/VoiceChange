#include <jni.h>
#include <string>
#include <unistd.h>
#include <fmod.hpp>

using namespace FMOD;

#undef com_zj_voicechange_MainActivity_MODE_NORMAL
#define com_zj_voicechange_MainActivity_MODE_NORMAL 0L
#undef com_zj_voicechange_MainActivity_MODE_LUOLI
#define com_zj_voicechange_MainActivity_MODE_LUOLI 1L
#undef com_zj_voicechange_MainActivity_MODE_DASHU
#define com_zj_voicechange_MainActivity_MODE_DASHU 2L
#undef com_zj_voicechange_MainActivity_MODE_JINGSONG
#define com_zj_voicechange_MainActivity_MODE_JINGSONG 3L
#undef com_zj_voicechange_MainActivity_MODE_GAOGUAI
#define com_zj_voicechange_MainActivity_MODE_GAOGUAI 4L
#undef com_zj_voicechange_MainActivity_MODE_KONGLING
#define com_zj_voicechange_MainActivity_MODE_KONGLING 5L

extern "C"
JNIEXPORT void JNICALL
Java_com_zj_voicechange_MainActivity_voiceChange(JNIEnv *env, jobject thiz, jint model,
                                                 jstring path) {
    char *content_ = "默认 播放完毕";

    //音频路径
    const char *path_ = env->GetStringUTFChars(path, NULL);

    // 音效引擎系统
    System *system = 0;
    // 声音指针
    Sound *sound = 0;
    // 通道
    Channel *channel = 0;
    // DSP
    DSP *dsp = 0;

    // 1.创建系统
    System_Create(&system);
    // 2.系统初始化  参数1：最大音轨数，  参数2：系统初始化标记， 参数3：额外数据
    system->init(32, FMOD_INIT_NORMAL, 0);
    // 3.创建声音 参数1：路径，  参数2：声音初始化标记， 参数3：额外数据， 参数4：声音指针
    system->createSound(path_, FMOD_DEFAULT, 0, &sound);
    // 4.播放声音 参数1：声音 参数2：分组音轨 参数3：控制 参数4：通道
    system->playSound(sound, 0, false, &channel);
    switch (model) {
        case com_zj_voicechange_MainActivity_MODE_NORMAL:
            content_ = "原生 播放完毕";
            break;
        case com_zj_voicechange_MainActivity_MODE_LUOLI:
            content_ = "萝莉 播放完毕";

            // 音调高 --- 萝莉 2.0
            // 1.创建DSP类型的Pitch 
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
            // 2.设置Pitch音调
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 2.0F);
            // 3.添加音效到音轨
            channel->addDSP(0, dsp);
            break;
        case com_zj_voicechange_MainActivity_MODE_DASHU:
            content_ = "大叔 播放完毕";
            // 音调低 --- 萝莉 2.0
            // 1.创建DSP类型的Pitch
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
            // 2.设置Pitch音调
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.7f);
            // 3.添加音效到音轨
            channel->addDSP(0, dsp);

            break;
        case com_zj_voicechange_MainActivity_MODE_JINGSONG:
            content_ = "惊悚 播放完毕";

            // 惊悚很多音效的拼接
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT,&dsp);
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH,2.0F);
            channel->addDSP(0,dsp);

            // 回音 ECHO
            system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
            dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 200); // 回音 延时    to 5000.  Default = 500.
            dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 10); // 回音 衰减度 Default = 50   0 完全衰减了
            channel->addDSP(1,dsp); // 第二个音轨

            //创建音频处理DSP 类型 为颤抖
            system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &dsp);
            //设置低频震旦器频率
            dsp->setParameterFloat(FMOD_DSP_TREMOLO_FREQUENCY, 20);
            //设置声音歪斜
            dsp->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 0.5);

            //添加一个处理单元到通道
            channel->addDSP(2, dsp);
            break;
        case com_zj_voicechange_MainActivity_MODE_GAOGUAI:
            content_ = "搞怪 播放完毕";
            // 搞怪 频率快
            float mFrequency;
            channel->getFrequency(&mFrequency);
            // 修改频率
            channel->setFrequency(mFrequency * 1.5f);
            break;
        case com_zj_voicechange_MainActivity_MODE_KONGLING:
            content_ = "空灵 播放完毕";

            // 回音 ECHO
            system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
            dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 200); // 回音 延时    to 5000.  Default = 500.
            dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 10); // 回音 衰减度 Default = 50   0 完全衰减了
            channel->addDSP(0, dsp);
            break;
    }

    // 等待播放完毕 再回收
    bool isPlayer = true; // 你用不是一级指针  我用一级指针接收你，可以修改给你
    while (isPlayer) {
        channel->isPlaying(&isPlayer); // 如果真的播放完成了，音轨是知道的，内部会修改isPlayer=false
        usleep(1000 * 1000); // 每个一秒
    }
    // 回收
    sound->release();
    system->close();
    system->release();
    env->ReleaseStringUTFChars(path, path_);

    // 告知java播放完毕
    jclass mainClass = env->GetObjectClass(thiz);
    jmethodID endMethodId = env->GetMethodID(mainClass, "playerEnd", "(Ljava/lang/String;)V");
    jstring content = env->NewStringUTF(content_);
    env->CallVoidMethod(thiz, endMethodId, content);
}