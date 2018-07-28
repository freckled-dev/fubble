
package com.freckles.of.couple.fubble;

import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class ResponseCount {

    private static final Logger   LOGGER = LogManager.getLogger(ResponseCount.class);

    private static CountDownLatch countDownLatch;

    public static void startLatch(int responseCount) {
        countDownLatch = new CountDownLatch(responseCount);
    }

    public static void await() {
        try {
            boolean completed = countDownLatch.await(10, TimeUnit.SECONDS);
            assert completed : "Timeout for countDownLatch!";
        } catch (InterruptedException ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }
    }

    public static void countDown() {
        countDownLatch.countDown();
    }

}
