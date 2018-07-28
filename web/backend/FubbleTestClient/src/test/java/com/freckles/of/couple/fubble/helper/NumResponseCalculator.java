
package com.freckles.of.couple.fubble.helper;

import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class NumResponseCalculator {

    private static final Logger LOGGER = LogManager.getLogger(NumResponseCalculator.class);

    public static int numJoinRoomResponses(int alreadyInRoom) {
        int joinedRoom = 1;
        int informOthersAboutMe = alreadyInRoom + 1;
        int informMeAboutOthers = alreadyInRoom;

        int numServerResponses = joinedRoom + informOthersAboutMe + informMeAboutOthers;
        LOGGER.debug(String.format("We want %s join room server responses.", numServerResponses));
        return numServerResponses;
    }

    public static int numLockRoomResponses(int alreadyInRoom) {
        LOGGER.debug(String.format("We want %s lock room server responses.", alreadyInRoom));
        return alreadyInRoom;
    }

    public static int numRoomLockedResponses(int userJoining) {
        LOGGER.debug(String.format("We want %s room locked server responses.", userJoining));
        return userJoining;
    }

    public static int numChatResponses(int alreadyInRoom) {
        LOGGER.debug(String.format("We want %s chat message server responses.", alreadyInRoom));
        return alreadyInRoom;
    }

    public static int numUserLeftResponses(int leftInRoom) {
        LOGGER.debug(String.format("We want %s left room server responses.", leftInRoom));
        return leftInRoom;
    }

    public static int numRenameUserResponses(int alreadyInRoom) {
        LOGGER.debug(String.format("We want %s rename user server responses.", alreadyInRoom));
        return alreadyInRoom;
    }

}
