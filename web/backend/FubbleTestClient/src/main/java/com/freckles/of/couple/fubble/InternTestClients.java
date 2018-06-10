
package com.freckles.of.couple.fubble;

import java.io.IOException;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;
import org.junit.Ignore;
import org.junit.Test;

/**
 * Just for internal testing - spawning a console and be able to act as a Fubble client (server must be running)
 *
 */
public class InternTestClients extends WebsocketTest {

    private static final Logger LOGGER = LogManager.getLogger(InternTestClients.class);

    @Ignore
    @Test
    public void createClients() {
        Runtime runTime = Runtime.getRuntime();
        String command = "cmd /c start cmd.exe /K " // start the cmd
            + "\"mvn exec:java -D\"exec.mainClass\"=\"com.freckles.of.couple.fubble.FubbleMain\""; // execute the main

        try {
            runTime.exec(command);
        } catch (IOException ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

}
