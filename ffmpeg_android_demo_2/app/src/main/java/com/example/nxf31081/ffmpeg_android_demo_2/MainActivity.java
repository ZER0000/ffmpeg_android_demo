package com.example.nxf31081.ffmpeg_android_demo_2;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private TextView mTextView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        mTextView = (TextView) findViewById(R.id.sample_text);
    }

    public void onButtonClick(View view) {
        int id = view.getId();

        switch (id) {
            case R.id.button_protocol:
                mTextView.setText(avProtocolInfo());
                break;
            case R.id.button_codec:
                mTextView.setText(avCodecInfo());
                break;
            case R.id.button_filter:
                mTextView.setText(avFilterInfo());
                break;
            case R.id.button_format:
                mTextView.setText(avFormatInfo());
                break;
            case R.id.button_configuration:
                mTextView.setText(avConfigurationInfo());
                break;
        }
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String avProtocolInfo();

    public native String avFormatInfo();

    public native String avCodecInfo();

    public native String avFilterInfo();

    public native String avConfigurationInfo();
}