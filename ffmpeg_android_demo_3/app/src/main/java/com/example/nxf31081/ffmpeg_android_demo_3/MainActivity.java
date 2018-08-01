package com.example.nxf31081.ffmpeg_android_demo_3;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //Set sdcard permission
        PermissionUtils.verifyStoragePermissions(this);

        Button startButton = (Button) this.findViewById(R.id.button_start);
        final EditText urlEditText_input = (EditText) this.findViewById(R.id.input_url);
        final EditText urlEditText_output = (EditText) this.findViewById(R.id.output_url);

        startButton.setOnClickListener(new View.OnClickListener(){
            public void onClick(View v) {
                String folderurl = Environment.getExternalStorageDirectory().getPath();

                String urltext_input = urlEditText_input.getText().toString();
                String urltext_output = urlEditText_output.getText().toString();

                String inputurl = folderurl + "/" + urltext_input;
                String outputurl = folderurl + "/" + urltext_output;

                Log.i("inputurl", inputurl);
                Log.i("outputurl", outputurl);

                decode(inputurl, outputurl);
            }

        });
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native int decode(String inputurl, String ouputurl);
}
