package com.freckles.of.couple.fubble;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.freckles.of.couple.fubble.dao.RoomDao;
import com.freckles.of.couple.fubble.entity.FubbleRoom;
import com.freckles.of.couple.fubble.tools.DatabaseHolder;

import java.util.Collections;
import java.util.Comparator;
import java.util.Date;

public class RoomActivity extends Activity {

    public static String EXTRA_ROOM_NAME = "roomName";

    private TextView tvRoomName;
    private FubbleRoom room;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_room);

        Intent intent = getIntent();
        String roomName = intent.getStringExtra(EXTRA_ROOM_NAME);
        tvRoomName = findViewById(R.id.tv_activity_room_name);

        new GetRoomAsync(roomName).execute();
    }


    @Override
    public void onPause() {
        super.onPause();
        Log.i(RoomActivity.class.getName(), "here");
    }

    public void setRoom(FubbleRoom room) {
        this.room = room;
    }


    private class GetRoomAsync extends AsyncTask<Void, Void, FubbleRoom> {

        private String roomName;

        public GetRoomAsync(String roomName) {
            this.roomName = roomName;
        }

        @Override
        protected FubbleRoom doInBackground(Void... params) {
            return DatabaseHolder.database.roomDao().findByName(roomName);
        }

        @Override
        protected void onPostExecute(FubbleRoom room) {
            setRoom(room);
            tvRoomName.setText(room.getName());
            new UpdateRoomAsync(room).execute();
        }
    }

    private class UpdateRoomAsync extends AsyncTask<Void, Void, FubbleRoom> {

        private FubbleRoom room;

        public UpdateRoomAsync(FubbleRoom room) {
            this.room = room;
        }

        @Override
        protected FubbleRoom doInBackground(Void... params) {
            RoomDao roomDao = DatabaseHolder.database.roomDao();
            room.setLastConnected(new Date());
            roomDao.updateRoom(room);
            return room;
        }

        @Override
        protected void onPostExecute(FubbleRoom updatedRoom) {
            room = updatedRoom;
        }

    }

}
