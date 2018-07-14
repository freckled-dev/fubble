
package com.freckles.of.couple.fubble.tools;

import java.io.IOException;
import java.util.Properties;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class FubbleProperties {

    private static final Logger     LOGGER           = LogManager.getLogger(FubbleProperties.class);

    private static final String     USER_NAME_PREFIX = "user.name.prefix";
    private static final String     RTC_SERVER_HOST  = "rtc.server.host";
    private static final String     RTC_SERVER_PORT  = "rtc.server.port";

    private static FubbleProperties instance         = null;
    private Properties              properties;

    private String                  userNamePrefix;
    private String                  rtcServerHost;
    private int                     rtcServerPort;

    private FubbleProperties() {
        properties = new Properties();

        try {
            properties.load(getClass().getClassLoader().getResourceAsStream("config.properties"));
        } catch (IOException ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }

        userNamePrefix = getValue(USER_NAME_PREFIX);
        rtcServerHost = getValue(RTC_SERVER_HOST);
        rtcServerPort = getIntValue(RTC_SERVER_PORT);
    }

    public String getUserNamePrefix() {
        return userNamePrefix;
    }

    public String getRtcServerHost() {
        return rtcServerHost;
    }

    public int getRtcServerPort() {
        return rtcServerPort;
    }

    public static FubbleProperties getInstance() {
        if (instance == null) {
            instance = new FubbleProperties();
        }
        return instance;
    }

    public String getValue(String key) {
        return properties.getProperty(key);
    }

    private Integer getIntValue(String key) {
        String property = properties.getProperty(key);
        return Integer.valueOf(property);
    }

}