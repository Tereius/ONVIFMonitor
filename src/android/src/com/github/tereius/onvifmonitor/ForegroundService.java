package com.github.tereius.onvifmonitor;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.os.Bundle;
import android.os.Build;
import android.os.IBinder;
import androidx.core.app.NotificationCompat;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import org.qtproject.qt.android.bindings.QtService;

public class ForegroundService extends QtService
{
    private static final String TAG = "QtAndroidService";
    public static final String CHANNEL_ID = "ForegroundServiceChannel";

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        boolean stopForeground = false;
        Bundle extras = intent.getExtras();
        if(extras != null && extras.containsKey("STOP_FOREGROUND_SERVICE") == true) {
            stopForeground = true;
        }

        Log.i(TAG, "About onStartCommand");
        super.onStartCommand(intent, flags, startId);

        Notification notification = createNotification();
        startForeground(123, notification);

        if(stopForeground) {
            Log.i(TAG, "About onStop");
            stopForeground(true);
            stopSelf();
        }
        return Service.START_STICKY;
    }

/*
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }*/

    @Override
    public void onCreate() {
        Log.i(TAG, "About Creating Service");
        createNotificationChannel();

        //startForeground(1, notification);
        super.onCreate();
        //don't call super.onCreate() here - will block and onStartCommand() is not called.
        Log.i(TAG, "Creating Service");
    }
/*
    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.i(TAG, "Destroying Service");
    }
*/
    private Notification createNotification() {

        Intent notificationIntent = new Intent(this, com.github.tereius.onvifmonitor.Activity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this, 0, notificationIntent, PendingIntent.FLAG_IMMUTABLE);
        return new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("Blaa")
                .setContentText("asdfasdfa")
                .setSmallIcon(R.drawable.ic_launcher)
                .setContentIntent(pendingIntent)
                .setOngoing(true)
                .setOnlyAlertOnce(true)
                .setSilent(true)
                .setForegroundServiceBehavior(NotificationCompat.FOREGROUND_SERVICE_DEFERRED)
                .setNumber(0)
                .build();
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel serviceChannel = new NotificationChannel(
                    CHANNEL_ID,
                    "Foreground Service Channel",
                    NotificationManager.IMPORTANCE_DEFAULT
            );
            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(serviceChannel);
            Log.i(TAG, "Created notification channel");
        }
    }
}