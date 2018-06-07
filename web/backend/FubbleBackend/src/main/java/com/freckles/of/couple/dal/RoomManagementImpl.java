
package com.freckles.of.couple.dal;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import com.freckles.of.couple.fubble.entities.Room;

public class RoomManagementImpl implements RoomManagementDAO {

    private static final Logger LOGGER = LogManager.getLogger(RoomManagementImpl.class);

    private static List<Room>   rooms  = new ArrayList<>();

    @Override
    public void deleteRoom(String roomName) {
        synchronized (rooms) {
            Room found = rooms.stream() //
                .filter(room -> room.getName().equals(roomName)) //
                .findFirst().orElse(null);
            if (found != null) {
                rooms.remove(found);
                LOGGER.info(String.format("Server: room %s has been deleted.", roomName));
            }
        }
    }

    @Override
    public Room readOrCreateRoom(String roomName) {
        synchronized (rooms) {
            Room existing = rooms.stream().filter(room -> room.getName().equals(roomName)).findFirst().orElse(null);

            if (existing == null) {
                existing = new Room(roomName);
                existing.setId(UUID.randomUUID().toString());
                rooms.add(existing);
                LOGGER.info(String.format("Server: Room %s has been created.", roomName));
            } else {
                // LOGGER.info(String.format("Server: Room %s already exists.", roomName));
            }

            return existing;
        }
    }

}
