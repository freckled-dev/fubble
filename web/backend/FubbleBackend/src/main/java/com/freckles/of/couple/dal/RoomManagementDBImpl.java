
package com.freckles.of.couple.dal;

import java.util.UUID;

import javax.persistence.criteria.CriteriaBuilder;
import javax.persistence.criteria.CriteriaQuery;
import javax.persistence.criteria.Root;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.hibernate.Session;

import com.freckles.of.couple.fubble.entities.Room;
import com.freckles.of.couple.fubble.entities.Room_;
import com.freckles.of.couple.fubble.tools.HibernateUtil;

public class RoomManagementDBImpl {

    private static final Logger LOGGER = LogManager.getLogger(RoomManagementDBImpl.class);

    public void deleteRoom(String roomName) {
        Session session = HibernateUtil.getSessionFactory().openSession();
        Room room = readRoom(session, roomName);

        if (room != null) {
            session.beginTransaction();
            session.delete(room);
            session.getTransaction().commit();

            LOGGER.info(String.format("Server: room %s has been deleted.", roomName));
        }
    }

    public Room readOrCreateRoom(String roomName) {
        Session session = HibernateUtil.getSessionFactory().openSession();

        Room room = readRoom(session, roomName);

        if (room == null) {
            room = createRoom(session, roomName);
        }

        return room;
    }

    private Room createRoom(Session session, String roomName) {
        session.beginTransaction();

        Room room = new Room(roomName);
        room.setId(UUID.randomUUID().toString());

        session.save(room);
        session.getTransaction().commit();

        LOGGER.info(String.format("Server: Room %s has been created.", roomName));

        return room;
    }

    private Room readRoom(Session session, String roomName) {
        CriteriaBuilder builder = session.getCriteriaBuilder();

        CriteriaQuery<Room> query = builder.createQuery(Room.class);
        Root<Room> root = query.from(Room.class);
        query.select(root);
        query.where(builder.equal(root.get(Room_.name), roomName));

        return session.createQuery(query).getSingleResult();
    }

}
