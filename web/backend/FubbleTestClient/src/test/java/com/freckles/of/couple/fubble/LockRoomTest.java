
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.List;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

import com.freckles.of.couple.fubble.proto.WebContainer.JoinedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.LockRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.LockedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

public class LockRoomTest extends WebsocketTest {

    private static final Logger LOGGER         = LogManager.getLogger(LockRoomTest.class);

    private static final String ROOM_NAME_1    = "dagmar-test-1";
    private static final String ROOM_NAME_2    = "dagmar-test-2";
    private static final String ROOM_NAME_3    = "dagmar-test-3";
    private static final String ROOM_NAME_4    = "dagmar-test-4";

    private static final int    WAITING_PERIOD = 1000;

    @Test
    public void testReceiveLockedRoom() {
        try {
            // 3 clients join
            List<FubbleClientEndpoint> clients = createClients(3);

            for (FubbleClientEndpoint client : clients) {
                joinRoom(ROOM_NAME_1, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // a client locks the room
            lockRoom("SuperSecurePassword1234", clients.get(0));

            Thread.sleep(WAITING_PERIOD);

            // all clients should get the message - room is locked
            for (FubbleClientEndpoint client : clients) {
                List<MessageContainerClient> messages = client.getMessages();

                boolean lockedRoomReceived = false;
                for (MessageContainerClient message : messages) {
                    if (MessageTypeCase.LOCK_ROOM.equals(message.getMessageTypeCase())) {
                        LockedRoom lockedRoom = message.getLockRoom();
                        if (lockedRoom.getLock()) {
                            lockedRoomReceived = true;
                        }
                    }
                }

                assertTrue(lockedRoomReceived);
            }

            Thread.sleep(WAITING_PERIOD);

            // a client unlocks the room
            unlockRoom(clients.get(1));

            Thread.sleep(WAITING_PERIOD);

            // all clients should get the message - room is unlocked
            for (FubbleClientEndpoint client : clients) {
                List<MessageContainerClient> messages = client.getMessages();

                boolean unlockedRoomReceived = false;
                for (MessageContainerClient message : messages) {
                    if (MessageTypeCase.LOCK_ROOM.equals(message.getMessageTypeCase())) {
                        LockedRoom lockedRoom = message.getLockRoom();
                        if (!lockedRoom.getLock()) {
                            unlockedRoomReceived = true;
                        }
                    }
                }

                assertTrue(unlockedRoomReceived);
            }

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    @Test
    public void testJoiningLockedRoom() {
        try {
            // 3 clients join
            List<FubbleClientEndpoint> clients = createClients(3);

            for (FubbleClientEndpoint client : clients) {
                joinRoom(ROOM_NAME_2, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // a client locks the room
            lockRoom("Password123", clients.get(0));

            Thread.sleep(WAITING_PERIOD);

            // a new client wants to join the locked room
            FubbleClientEndpoint newJoin = createClient();
            joinRoom(ROOM_NAME_2, newJoin);

            Thread.sleep(WAITING_PERIOD);
            assertTrue(newJoin.getMessages().stream() //
                .filter(message -> message.getMessageTypeCase().equals(MessageTypeCase.ERROR)) //
                .findFirst().isPresent());

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    @Test
    public void testJoinRoomAfterError() {
        try {
            // 3 clients join
            List<FubbleClientEndpoint> clients = createClients(3);

            for (FubbleClientEndpoint client : clients) {
                joinRoom(ROOM_NAME_3, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // a client locks the room
            String password = "HalliHallo";
            lockRoom(password, clients.get(0));

            Thread.sleep(WAITING_PERIOD);

            // a new client wants to join the locked room
            FubbleClientEndpoint newJoin = createClient();
            joinRoom(ROOM_NAME_3, newJoin);

            Thread.sleep(WAITING_PERIOD);
            assertTrue(newJoin.getMessages().stream() //
                .filter(message -> message.getMessageTypeCase().equals(MessageTypeCase.ERROR)) //
                .findFirst().isPresent());

            Thread.sleep(WAITING_PERIOD);

            // the new client tries again - now with password
            joinRoom(ROOM_NAME_3, newJoin, password);

            Thread.sleep(WAITING_PERIOD);

            // verify if he got in
            boolean joinedRoomReceived = false;

            for (MessageContainerClient message : newJoin.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.JOINED_ROOM.equals(type)) {
                    JoinedRoom joined = message.getJoinedRoom();
                    if (joined.getUserId().equals(newJoin.getUserId())) {
                        joinedRoomReceived = true;
                    }
                }
            }

            assertTrue(joinedRoomReceived);

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    @Test
    public void testJoinRoomWithPassword() {
        try {
            // 3 clients join
            List<FubbleClientEndpoint> clients = createClients(3);

            for (FubbleClientEndpoint client : clients) {
                joinRoom(ROOM_NAME_4, client);
            }

            Thread.sleep(WAITING_PERIOD);

            // a client locks the room
            String password = "HalliHallo";
            lockRoom(password, clients.get(0));

            Thread.sleep(WAITING_PERIOD);

            // a new client wants to join the locked room - he has the password and sends this immediately
            FubbleClientEndpoint newJoin = createClient();
            joinRoom(ROOM_NAME_4, newJoin, password);

            // no error
            Thread.sleep(WAITING_PERIOD);
            assertFalse(newJoin.getMessages().stream() //
                .filter(message -> message.getMessageTypeCase().equals(MessageTypeCase.ERROR)) //
                .findFirst().isPresent());

            // verify if he got in
            boolean joinedRoomReceived = false;

            for (MessageContainerClient message : newJoin.getMessages()) {
                MessageTypeCase type = message.getMessageTypeCase();
                if (MessageTypeCase.JOINED_ROOM.equals(type)) {
                    JoinedRoom joined = message.getJoinedRoom();
                    if (joined.getUserId().equals(newJoin.getUserId())) {
                        joinedRoomReceived = true;
                    }
                }
            }

            assertTrue(joinedRoomReceived);

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    private void lockRoom(String password, FubbleClientEndpoint client) {
        LockRoom lockRoom = LockRoom.newBuilder().setPassword(password).build();
        MessageContainerServer container = MessageContainerServer.newBuilder().setLockRoom(lockRoom).build();
        sendMessage(container, client);
    }

    private void unlockRoom(FubbleClientEndpoint client) {
        LockRoom lockRoom = LockRoom.newBuilder().setPassword("").build();
        MessageContainerServer container = MessageContainerServer.newBuilder().setLockRoom(lockRoom).build();
        sendMessage(container, client);
    }

}
