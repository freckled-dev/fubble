
package com.freckles.of.couple.fubble;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.util.List;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Assert;
import org.junit.Test;

import com.freckles.of.couple.fubble.helper.NumResponseCalculator;
import com.freckles.of.couple.fubble.proto.WebContainer.JoinedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.LockRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.LockedRoom;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerClient.MessageTypeCase;
import com.freckles.of.couple.fubble.proto.WebContainer.MessageContainerServer;

public class LockRoomTest extends WebsocketTestRunner {

    private static final Logger LOGGER      = LogManager.getLogger(LockRoomTest.class);

    private static final String ROOM_NAME_1 = "dagmar-test-1";
    private static final String ROOM_NAME_2 = "dagmar-test-2";
    private static final String ROOM_NAME_3 = "dagmar-test-3";
    private static final String ROOM_NAME_4 = "dagmar-test-4";

    @Test
    public void testReceiveLockedRoom() {
        try {
            // 3 clients join
            List<FubbleClientEndpoint> clients = createClients(3);

            for (int index = 0; index < clients.size(); index++) {
                FubbleClientEndpoint client = clients.get(index);
                ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(index));
                joinRoom(ROOM_NAME_1, client);
                ResponseCount.await();
            }

            // a client locks the room
            ResponseCount.startLatch(NumResponseCalculator.numLockRoomResponses(clients.size()));
            lockRoom("SuperSecurePassword1234", clients.get(0));
            ResponseCount.await();

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

            // a client unlocks the room
            ResponseCount.startLatch(NumResponseCalculator.numLockRoomResponses(clients.size()));
            unlockRoom(clients.get(1));
            ResponseCount.await();

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
            Assert.fail();
        }
    }

    @Test
    public void testJoiningLockedRoom() {
        try {
            // 3 clients join
            List<FubbleClientEndpoint> clients = createClients(3);

            for (int index = 0; index < clients.size(); index++) {
                FubbleClientEndpoint client = clients.get(index);
                ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(index));
                joinRoom(ROOM_NAME_2, client);
                ResponseCount.await();
            }

            // a client locks the room
            ResponseCount.startLatch(NumResponseCalculator.numLockRoomResponses(clients.size()));
            lockRoom("Password123", clients.get(0));
            ResponseCount.await();

            // a new client wants to join the locked room
            FubbleClientEndpoint newJoin = createClient();
            ResponseCount.startLatch(NumResponseCalculator.numRoomLockedResponses(1));
            joinRoom(ROOM_NAME_2, newJoin);
            ResponseCount.await();
            allClients.remove(newJoin);

            assertTrue(newJoin.getMessages().stream() //
                .filter(message -> message.getMessageTypeCase().equals(MessageTypeCase.ERROR)) //
                .findFirst().isPresent());

        } catch (Exception ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
            Assert.fail();
        }
    }

    @Test
    public void testJoinRoomAfterError() {
        try {
            // 3 clients join
            List<FubbleClientEndpoint> clients = createClients(3);

            for (int index = 0; index < clients.size(); index++) {
                FubbleClientEndpoint client = clients.get(index);
                ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(index));
                joinRoom(ROOM_NAME_3, client);
                ResponseCount.await();
            }

            // a client locks the room
            String password = "HalliHallo";
            ResponseCount.startLatch(NumResponseCalculator.numLockRoomResponses(clients.size()));
            lockRoom(password, clients.get(0));
            ResponseCount.await();

            // a new client wants to join the locked room
            FubbleClientEndpoint newJoin = createClient();
            ResponseCount.startLatch(NumResponseCalculator.numRoomLockedResponses(1));
            joinRoom(ROOM_NAME_3, newJoin);
            ResponseCount.await();
            allClients.remove(newJoin);

            assertTrue(newJoin.getMessages().stream() //
                .filter(message -> message.getMessageTypeCase().equals(MessageTypeCase.ERROR)) //
                .findFirst().isPresent());

            // the new client tries again - now with correct password
            ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(3));
            joinRoom(ROOM_NAME_3, newJoin, password, "");
            ResponseCount.await();
            allClients.add(newJoin);

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
            Assert.fail();
        }
    }

    @Test
    public void testJoinRoomWithPassword() {
        try {
            // 3 clients join
            List<FubbleClientEndpoint> clients = createClients(3);

            for (int index = 0; index < clients.size(); index++) {
                FubbleClientEndpoint client = clients.get(index);
                ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(index));
                joinRoom(ROOM_NAME_4, client);
                ResponseCount.await();
            }

            // a client locks the room
            String password = "HalliHallo";
            ResponseCount.startLatch(NumResponseCalculator.numLockRoomResponses(clients.size()));
            lockRoom(password, clients.get(0));
            ResponseCount.await();

            // a new client wants to join the locked room - he has the password and sends this immediately
            FubbleClientEndpoint newJoin = createClient();
            ResponseCount.startLatch(NumResponseCalculator.numJoinRoomResponses(3));
            joinRoom(ROOM_NAME_4, newJoin, password, "");
            ResponseCount.await();

            // no error
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
            Assert.fail();
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
