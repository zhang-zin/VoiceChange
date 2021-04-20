package com.zj.voicechange

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import android.view.View
import java.lang.NullPointerException

class JniTestActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_jni_test)
    }

    external fun dynamicMethod01()
    external fun dynamicMethod02(valueStr: String): Int

    // JNI线程
    external fun nativeThread()

    companion object {

        init {
            System.loadLibrary("native-lib")
        }
    }

    /**
     * 测试抛出异常
     *
     * @throws NullPointerException
     */
    @Throws(NullPointerException::class)
    private fun textException(){
        throw NullPointerException("JniTestActivity testException NullPointerException")
    }

    fun dynamic01(view: View) {
        dynamicMethod01()
    }

    fun dynamic02(view: View) {
        val dynamicMethod02 = dynamicMethod02("我是Java")
        Log.e("JNI_TEXT", "dynamicMethod02 result: $dynamicMethod02")
    }

    fun nativeCallJava(view: View) {
        nativeThread()
    }
    fun clickMethod4(view: View) {}
    fun clickMethod5(view: View) {}
}