package com.freckles.of.couple.fubble.tools;

import java.text.SimpleDateFormat;
import java.util.Date;

public class DateUtils {

    public static boolean isSameDay(Date date1, Date date2) {
        SimpleDateFormat fmt = new SimpleDateFormat("yyyyMMdd");
        return fmt.format(date1).equals(fmt.format(date2));
    }
}
