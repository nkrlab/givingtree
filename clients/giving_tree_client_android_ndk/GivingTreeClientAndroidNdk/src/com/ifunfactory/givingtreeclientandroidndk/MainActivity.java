package com.ifunfactory.givingtreeclientandroidndk;

import android.os.Bundle;
import android.app.Activity;
import android.view.Menu;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity {
  static {
    System.loadLibrary("gnustl_shared");
    System.loadLibrary("protobuf-android-ndk");
    System.loadLibrary("giving_tree_client-android-ndk");
  }

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    log("MainActivity.onCreate");
    start();
  }

  @Override
  public boolean onCreateOptionsMenu(Menu menu) {
    // Inflate the menu; this adds items to the action bar if it is present.
    getMenuInflater().inflate(R.menu.main, menu);
    return true;
  }

  public void onButtonClick(View view) {
    String caption = ((Button) view).getText().toString();
    log("MainActivity.onButtonClick: " + caption);
    command(caption);
  }

  private native void start();
  private native void log(String logThis);
  private native void command(String cmd);
}
