
package com.freckles.of.couple.dal.cache;

import java.util.ArrayList;
import java.util.List;

import com.freckles.of.couple.dal.RoomManagementDAO;
import com.freckles.of.couple.fubble.entities.Room;

public class RoomManagementDAOImpl implements RoomManagementDAO {

    private List<Room> rooms = new ArrayList<>();

    @Override
    public Room readRoom(String roomName) {
        return rooms.stream().filter(room -> room.getName().equals(roomName)).findFirst().orElse(null);
    }

    @Override
    public Room updateRoom(Room room) {
        deleteRoom(room.getName());
        rooms.add(room);
        return room;
    }

    @Override
    public Room createRoom(String roomName) {
        Room newRoom = new Room(roomName);
        rooms.add(newRoom);
        return newRoom;
    }

    @Override
    public void deleteRoom(String roomName) {
        Room found = rooms.stream() //
            .filter(room -> room.getName().equals(roomName)) //
            .findFirst().get();
        rooms.remove(found);
    }

}
