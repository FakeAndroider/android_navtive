package com.example.cplusplus;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.cplusplus.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'cplusplus' library on application startup.
    static {
        System.loadLibrary("cplusplus");
    }


    public native String stringFromJNI();

    /// 引擎启动
    public native boolean onEngineCreate();


    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        final boolean engineLunch = onEngineCreate();

        System.out.println("--->engine state:" + engineLunch);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
    }


}