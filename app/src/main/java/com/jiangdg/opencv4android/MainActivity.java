package com.jiangdg.opencv4android;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.jiangdg.opencv4android.imgproc.ImgProcessActivity;
import com.jiangdg.opencv4android.objdetect.FaceDetectActivity;

import butterknife.BindView;
import butterknife.ButterKnife;
import butterknife.OnClick;

public class MainActivity extends AppCompatActivity {
    @BindView(R.id.btn_hello)
    Button mBtnHello;
    @BindView(R.id.btn_faceDetect)
    Button mBtnFaceDetect;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // 绑定View
        ButterKnife.bind(this);
    }

    @OnClick({R.id.btn_hello,R.id.btn_faceDetect})
    public void onViewClick(View view) {
        int vId = view.getId();
        switch (vId) {
            case R.id.btn_hello:
                Intent intentHello = new Intent(MainActivity.this, ImgProcessActivity.class);
                startActivity(intentHello);
                break;
            case R.id.btn_faceDetect:
                Intent intentFd = new Intent(MainActivity.this,FaceDetectActivity.class);
                startActivity(intentFd);
                break;
        }
    }
}
