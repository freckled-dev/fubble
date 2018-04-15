
package com.freckles.of.couple.fubble.tools;

import java.io.IOException;
import java.util.Date;

import com.google.gson.TypeAdapter;
import com.google.gson.stream.JsonReader;
import com.google.gson.stream.JsonWriter;

public class DateTypeAdapter extends TypeAdapter<Date> {

    @Override
    public void write(JsonWriter out, Date value)
        throws IOException {
        if (value == null) {
            out.nullValue();
        } else {
            out.value(value.getTime() / 1000);
        }
    }

    @Override
    public Date read(JsonReader in)
        throws IOException {
        if (in != null) {
            long unixTimestamp = in.nextLong();
            return new Date(unixTimestamp * 1000);
        } else {
            return null;
        }
    }
}