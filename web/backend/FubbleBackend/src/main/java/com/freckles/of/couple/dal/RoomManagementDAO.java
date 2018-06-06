
package com.freckles.of.couple.dal;

import com.freckles.of.couple.fubble.entities.Room;

public interface RoomManagementDAO {

    public Room updateRoom(Room room);

    public void deleteRoom(String roomName);

    public Room readOrCreateRoom(String roomName);

}
