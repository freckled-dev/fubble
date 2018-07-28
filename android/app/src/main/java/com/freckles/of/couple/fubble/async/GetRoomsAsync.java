package com.freckles.of.couple.fubble.async;

import android.app.Activity;
import android.os.AsyncTask;

import com.freckles.of.couple.fubble.MainActivity;
import com.freckles.of.couple.fubble.entity.FubbleRoom;
import com.freckles.of.couple.fubble.tools.DatabaseHolder;

import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class GetRoomsAsync extends AsyncTask<Void, Void, List<FubbleRoom>> {

    private MainActivity mainActivity;

    public GetRoomsAsync(Activity mainActivity) {
        this.mainActivity = (MainActivity) mainActivity;
    }

    @Override
    protected List<FubbleRoom> doInBackground(Void... params) {
        return DatabaseHolder.database.roomDao().getAll();
    }

    @Override
    protected void onPostExecute(final List<FubbleRoom> rooms) {
        Collections.sort(rooms, new Comparator<FubbleRoom>() {
            @Override
            public int compare(FubbleRoom r1, FubbleRoom r2) {
                return r2.getLastConnected().compareTo(r1.getLastConnected());
            }
        });

        mainActivity.createRoomList(rooms);

    }
}