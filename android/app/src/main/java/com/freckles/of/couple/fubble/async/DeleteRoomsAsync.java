package com.freckles.of.couple.fubble.async;

import android.os.AsyncTask;

import com.freckles.of.couple.fubble.entity.FubbleRoom;
import com.freckles.of.couple.fubble.tools.DatabaseHolder;
import com.freckles.of.couple.fubble.tools.RoomListAdapter;

public class DeleteRoomsAsync extends AsyncTask<Void, Void, Void> {

        private int position;
        private RoomListAdapter adapter;

        public DeleteRoomsAsync(int position, RoomListAdapter adapter) {
            this.position = position;
            this.adapter = adapter;
        }

        @Override
        protected Void doInBackground(Void... params) {
            FubbleRoom room = adapter.getDataSet().get(position);
            DatabaseHolder.database.roomDao().delete(room);
            return null;
        }

        @Override
        protected void onPostExecute(Void param) {
            adapter.getDataSet().remove(position);
            adapter.notifyDataSetChanged();
        }

    }