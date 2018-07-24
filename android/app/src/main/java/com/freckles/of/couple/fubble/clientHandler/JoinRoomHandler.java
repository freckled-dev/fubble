package com.freckles.of.couple.fubble.clientHandler;

import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.freckles.of.couple.fubble.RoomActivity;
import com.freckles.of.couple.fubble.dao.RoomDao;
import com.freckles.of.couple.fubble.entity.FubbleRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.tools.DatabaseHolder;

import java.util.Date;

public class JoinRoomHandler implements MessageHandler {

    private Context context;


    @Override
    public void handleMessage(Context context, MessageContainerClient container) {
        this.context = context;

        JoinedRoom joinedRoom = container.getJoinedRoom();
        Log.i(MessageSwitch.class.getName(), joinedRoom.toString());

        updateOrInsertRoom(joinedRoom);
        openRoomActivity(joinedRoom);
    }

    private void openRoomActivity(JoinedRoom room) {
        Intent intent = new Intent();
        intent.setClass(context, RoomActivity.class);
        intent.putExtra(RoomActivity.EXTRA_ROOM_NAME, room.getRoomName());
        context.startActivity(intent);
    }

    private void updateOrInsertRoom(JoinedRoom joinedRoom) {
        String roomName = joinedRoom.getRoomName();

        RoomDao roomDao = DatabaseHolder.database.roomDao();
        FubbleRoom room = roomDao.findByName(roomName);
        if (room == null) {
            room = createRoom(joinedRoom);
            roomDao.insert(room);
        } else {
            room.setLastConnected(new Date());
            roomDao.updateRoom(room);
        }
    }

    private FubbleRoom createRoom(JoinedRoom joinedRoom) {
        FubbleRoom newRoom = new FubbleRoom();
        newRoom.setName(joinedRoom.getRoomName());
        newRoom.setRoomId(joinedRoom.getRoomId());
        newRoom.setLastConnected(new Date());

        return newRoom;
    }
}
