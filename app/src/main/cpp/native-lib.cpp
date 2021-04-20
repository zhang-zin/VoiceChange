#include <jni.h>
#include <string>
#include <unistd.h>
#include <fmod.hpp>
#include <android/log.h>
#include <pthread.h>

using namespace FMOD;

#define TAG "JNI_TEXT"
// __VA_ARGS__ 代表 ...的可变参数
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG,  __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG,  __VA_ARGS__);
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG,  __VA_ARGS__);

//region 变声类型宏定义
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
//endregion

JavaVM *jvm = nullptr;

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
            system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
            dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 2.0F);
            channel->addDSP(0, dsp);

            // 回音 ECHO
            system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
            dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 200); // 回音 延时    to 5000.  Default = 500.
            dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 10); // 回音 衰减度 Default = 50   0 完全衰减了
            channel->addDSP(1, dsp); // 第二个音轨

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

void dynamicMethod01(JNIEnv *env, jobject instance) {
    LOGE("动态注册方法：dynamicMethod01")

    jclass pJclass = env->GetObjectClass(instance);
    jmethodID textExceptionId = env->GetMethodID(pJclass, "textException", "()V");
    env->CallVoidMethod(instance, textExceptionId);
    jthrowable throwable = env->ExceptionOccurred(); // 判断是否发生异常
    if (throwable) {
        //发生异常，需要主动处理
        env->ExceptionDescribe(); // 打印异常信息
        env->ExceptionClear(); // 清除异常信息
        jclass aClass = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(aClass, "JNI 中发生了一个异常"); //返回一个新的异常到java
    }
}

int dynamicMethod02(JNIEnv *env, jobject jobject, jstring value) {
    const char *string = env->GetStringUTFChars(value, NULL);
    LOGE("动态注册方法，输出来自Java的字符串: %s", string);
    env->ReleaseStringUTFChars(value, string);
    return 1;
}

static const JNINativeMethod jniNativeMethods[] = {
        {"dynamicMethod01", "()V",                   (void *) (dynamicMethod01)},
        {"dynamicMethod02", "(Ljava/lang/String;)I", (int *) (dynamicMethod02)}
};


extern "C"
JNIEXPORT jint JNI_OnLoad(JavaVM *javaVm, void *) {
    //会默认调用这个方法

    jvm = javaVm;
    JNIEnv *jniEnv = nullptr;
    int result = javaVm->GetEnv(reinterpret_cast<void **>(&jniEnv), JNI_VERSION_1_6);
    LOGE("GetEnv: %d", result)

    if (result != JNI_OK) {
        return -1;
    }

    LOGE("获取JNI成功")

    char *registerName = "com/zj/voicechange/JniTestActivity";

    jclass pJclass = jniEnv->FindClass(registerName);
    jniEnv->RegisterNatives(pJclass, jniNativeMethods,
                            sizeof(jniNativeMethods) / sizeof(JNINativeMethod));

    return JNI_VERSION_1_6;
}

/**
 * JavaVM全局，绑定当前进程，只有一个地址
 * JNIEnv线程绑定，多个线程有多个地址，线程间不能互相调用
 * jobject 谁调用的jni函数，谁的实例就会给jobject
 */
void *threadTaskAction(void *pVoid) {
    /**
     * 只有JavaVm所有线程都可以直接调用
     * JNIEnv需要使用jvm->AttachCurrentThread(&jniEnv, nullptr)获取
     * jobject传递时需要提示了全局变量
     */
    JNIEnv *jniEnv = nullptr;
    int attachResult = jvm->AttachCurrentThread(&jniEnv, nullptr);

    if (attachResult != JNI_OK) {
        return 0;
    }

    jobject jobject1 = static_cast<jobject>(pVoid);
    jclass pJclass = jniEnv->GetObjectClass(jobject1);

    jniEnv->DeleteGlobalRef(jobject1);
    jvm->DetachCurrentThread();
    LOGE("JNI开启线程")
    return nullptr;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zj_voicechange_JniTestActivity_nativeThread(JNIEnv *env, jobject thiz) {
    pthread_t pid;
    jobject pJobject = env->NewGlobalRef(thiz);
    pthread_create(&pid, nullptr, threadTaskAction, pJobject);
    pthread_join(pid, nullptr);
}