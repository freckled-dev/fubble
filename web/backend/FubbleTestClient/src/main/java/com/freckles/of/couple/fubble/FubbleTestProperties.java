
package com.freckles.of.couple.fubble;

import java.io.IOException;
import java.util.Properties;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class FubbleTestProperties {

    private static final Logger         LOGGER           = LogManager.getLogger(FubbleTestProperties.class);

    private static final String         SERVER_LOCATION  = "fubble.server.location";
    private static final String         USER_NAME_PREFIX = "user.name.prefix";

    private static FubbleTestProperties instance         = null;
    private Properties                  properties;

    private String                      serverLocation;
    private String                      userNamePrefix;

    private FubbleTestProperties() {
        properties = new Properties();

        try {
            properties.load(getClass().getClassLoader().getResourceAsStream("config.properties"));
        } catch (IOException ex) {
            LOGGER.error(ExceptionUtils.getStackTrace(ex));
        }

        serverLocation = getValue(SERVER_LOCATION);
        userNamePrefix = getValue(USER_NAME_PREFIX);

    }

    public static FubbleTestProperties getInstance() {
        if (instance == null) {
            instance = new FubbleTestProperties();
        }
        return instance;
    }

    public String getValue(String key) {
        return properties.getProperty(key);
    }

    public String getServerLocation() {
        return serverLocation;
    }

    public String getUserNamePrefix() {
        return userNamePrefix;
    }

}