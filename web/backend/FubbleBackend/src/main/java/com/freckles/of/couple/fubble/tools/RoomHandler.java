
package com.freckles.of.couple.fubble.tools;

import java.util.ArrayList;
import java.util.List;

import javax.websocket.Session;

import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.User;

public class RoomHandler {

    private static final RoomHandler INSTANCE = new RoomHandler();

    private List<Room>               rooms    = new ArrayList<>();

    private RoomHandler() {

    }

    public List<Room> getRooms() {
        return rooms;
    }

    public static RoomHandler getInstance() {
        return INSTANCE;
    }

    public Room getRoomForSession(Session session) {
        for (Room room : rooms) {
            for (User user : room.getUsers()) {
                if (user.getSession().equals(session)) {
                    return room;
                }
            }
        }

        return null;
    }

    public User getUserForSession(Session session) {
        for (Room room : rooms) {
            for (User user : room.getUsers()) {
                if (user.getSession().equals(session)) {
                    return user;
                }
            }
        }

        return null;
    }

}
