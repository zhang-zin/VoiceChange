package com.zj.voicechange

import android.R.attr
import android.os.Bundle
import android.view.View
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import org.fmod.FMOD

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        FMOD.init(this)
    }

    override fun onDestroy() {
        super.onDestroy()
        FMOD.close()
    }

    fun onFix(view: View) {
        when (view.id) {
            R.id.btn_normal -> voiceChange(MODE_NORMAL,path) // 真实开发中，必须子线程  JNI线程（很多坑）
            R.id.btn_luoli -> voiceChange(MODE_LUOLI, path)
            R.id.btn_dashu -> voiceChange(MODE_DASHU, path)
            R.id.btn_jingsong -> voiceChange(MODE_JINGSONG, path)
            R.id.btn_gaoguai -> voiceChange(MODE_GAOGUAI, path)
            R.id.btn_kongling -> voiceChange(MODE_KONGLING, path)
        }
    }

    external fun voiceChange(model: Int, path: String)

    fun playerEnd(msg: String) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show()
    }

    companion object {

        const val path = "file:///android_asset/derry.mp3"

        private const val MODE_NORMAL = 0 // 正常

        private const val MODE_LUOLI = 1 //

        private const val MODE_DASHU = 2 //

        private const val MODE_JINGSONG = 3 //

        private const val MODE_GAOGUAI = 4 //

        private const val MODE_KONGLING = 5 //

        init {
            System.loadLibrary("native-lib")
        }
    }
}