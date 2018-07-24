package com.freckles.of.couple.fubble.tools;

import android.app.Activity;
import android.os.AsyncTask;
import android.support.annotation.NonNull;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.freckles.of.couple.fubble.R;
import com.freckles.of.couple.fubble.entity.FubbleRoom;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.concurrent.TimeUnit;

public class RoomListAdapter extends RecyclerView.Adapter<RoomListAdapter.ViewHolder> implements RecyclerView.OnClickListener {

    private SimpleDateFormat simpleDateFormat;

    private List<FubbleRoom> dataSet;
    private Activity activity;

    public static class ViewHolder extends RecyclerView.ViewHolder {
        TextView tvRoomName, tvLastConnected;
        LinearLayout background;

        ViewHolder(View itemView) {
            super(itemView);

            tvRoomName = itemView.findViewById(R.id.tv_room_name);
            tvLastConnected = itemView.findViewById(R.id.tv_last_connected);
            background = itemView.findViewById(R.id.ll_room_row);
        }

        public LinearLayout getBackground() {
            return background;
        }
    }

    public RoomListAdapter(List<FubbleRoom> data, Activity activity) {
        this.dataSet = data;
        this.activity = activity;

        String pattern = activity.getResources().getString(R.string.date_time_format);
        simpleDateFormat = new SimpleDateFormat(pattern);
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.rooms_row_item, parent, false);
        return new ViewHolder(view);
    }

    private FubbleRoom getItem(int position) {
        return dataSet.get(position);
    }

    @Override
    public void onBindViewHolder(@NonNull ViewHolder holder, int position) {
        FubbleRoom room = getItem(position);

        holder.tvRoomName.setText(room.getName());
        holder.tvLastConnected.setText(getLastConnectedText(room.getLastConnected()));
    }

    @Override
    public int getItemCount() {
        return dataSet.size();
    }


    @Override
    public void onClick(View view) {
        int position = (Integer) view.getTag();
        Object object = getItem(position);
        FubbleRoom room = (FubbleRoom) object;
        openRoomActivity(room);
    }


    private void openRoomActivity(FubbleRoom room) {
        JoinRoomHandler handler = new JoinRoomHandler(activity);
        handler.joinRoom(room.getName(), "");
    }

    private String getLastConnectedText(Date connected) {
        Date now = new Date();
        if (DateUtils.isSameDay(now, connected)) {
            long diff = now.getTime() - connected.getTime();
            long hours = TimeUnit.MILLISECONDS.toHours(diff);

            if (hours >= 1) {
                if (hours == 1) {
                    return String.format("%s%s%s%s", activity.getResources().getString(R.string.tv_connected_before), hours, activity.getResources().getString(R.string.tv_connected_hour), activity.getResources().getString(R.string.tv_connected_after));
                }

                return String.format("%s%s%s%s", activity.getResources().getString(R.string.tv_connected_before), hours, activity.getResources().getString(R.string.tv_connected_hours), activity.getResources().getString(R.string.tv_connected_after));
            } else {
                long minutes = TimeUnit.MILLISECONDS.toMinutes(diff);

                if (minutes == 1) {
                    return String.format("%s%s%s%s", activity.getResources().getString(R.string.tv_connected_before), minutes, activity.getResources().getString(R.string.tv_connected_minute), activity.getResources().getString(R.string.tv_connected_after));
                }

                return String.format("%s%s%s%s", activity.getResources().getString(R.string.tv_connected_before), minutes, activity.getResources().getString(R.string.tv_connected_minutes), activity.getResources().getString(R.string.tv_connected_after));
            }
        }

        return simpleDateFormat.format(connected);
    }

    public void onItemDismiss(int position) {
        new DeleteRoomsAsync(position).execute();
    }


    private class DeleteRoomsAsync extends AsyncTask<Void, Void, Void> {

        private int position;

        public DeleteRoomsAsync(int position) {
            this.position = position;
        }

        @Override
        protected Void doInBackground(Void... params) {
            FubbleRoom room = dataSet.get(position);
            DatabaseHolder.database.roomDao().delete(room);
            return null;
        }

        @Override
        protected void onPostExecute(Void param) {
            dataSet.remove(position);
            notifyDataSetChanged();
        }

    }

}