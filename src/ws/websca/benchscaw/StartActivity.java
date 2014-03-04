package ws.websca.benchscaw;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.text.ClipboardManager;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class StartActivity extends Activity {

	Clip[] clips;
	int currentClip=0;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_start);
	}


	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		//getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	public void onStart() {
		super.onStart();

		Button FFMPEGbutton = (Button)findViewById(R.id.FFMPEGButton);
		FFMPEGbutton.setOnClickListener(new OnClickListener() {
			public void onClick(View arg0) {
 			  Intent intent = new Intent(StartActivity.this, MainActivity.class);
 			  intent.putExtra("ws.websca.benchscaw.use", "ffmpeg");
			  startActivity(intent);
 			}
 		});
		
		Button MediaCodecButton = (Button)findViewById(R.id.mediaCodecButton);
		if(Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN){
			MediaCodecButton.setEnabled(false);
		}
		MediaCodecButton.setOnClickListener(new OnClickListener() {
			public void onClick(View arg0) {
 			  Intent intent = new Intent(StartActivity.this, MainActivity.class);
 			  intent.putExtra("ws.websca.benchscaw.use", "mediacodec");
			  startActivity(intent);
 			} 
		});
		
		Button noTestButton = (Button)findViewById(R.id.noTestButton);
		noTestButton.setOnClickListener(new OnClickListener() {
			public void onClick(View arg0) {
 			  Intent intent = new Intent(StartActivity.this, MainActivity.class);
 			  intent.putExtra("ws.websca.benchscaw.use", "notest");
			  startActivity(intent);
 			}
		});
	}
}
