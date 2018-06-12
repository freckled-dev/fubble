
package com.freckles.of.couple.fubble;

import java.io.IOException;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class InternTests {

    private static final Logger LOGGER      = LogManager.getLogger(InternTests.class);
    private static final int    NUM_CLIENTS = 1;

    public static void main(String[] args) {

        for (int index = 0; index < NUM_CLIENTS; index++) {
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

}
