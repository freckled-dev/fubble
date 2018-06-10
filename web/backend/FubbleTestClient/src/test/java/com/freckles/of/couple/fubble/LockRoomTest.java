
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertTrue;

import java.util.List;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Test;

import com.freckles.of.couple.fubble.proto.WebContainer.LockRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.LockedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

public class LockRoomTest extends WebsocketTest {

    private static final Logger LOGGER         = LogManager.getLogger(LockRoomTest.class);

    private static final String ROOM_NAME_1    = "dagmar-test-1";
    private static final String ROOM_NAME_2    = "dagmar-test-2";

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
            lockRoom(true, clients.get(0));

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
            lockRoom(false, clients.get(1));

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

            Thread.sleep(WAITING_PERIOD);

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
            lockRoom(true, clients.get(0));

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

    private void lockRoom(boolean lock, FubbleClientEndpoint client) {
        LockRoom lockRoom = LockRoom.newBuilder().setLock(lock).build();
        MessageContainerServer container = MessageContainerServer.newBuilder().setLockRoom(lockRoom).build();
        sendMessage(container, client);
    }

}
