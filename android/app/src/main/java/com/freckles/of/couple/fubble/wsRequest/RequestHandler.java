package com.freckles.of.couple.fubble.wsRequest;

import android.app.Activity;
import android.support.design.widget.Snackbar;

import com.freckles.of.couple.fubble.R;

import org.java_websocket.exceptions.WebsocketNotConnectedException;

class RequestHandler {

    public void handleError(Activity activity, Exception ex) {
        if (ex instanceof WebsocketNotConnectedException) {
            String errorText = activity.getResources().getString(R.string.web_server_error);
            Snackbar errorBar = Snackbar.make(activity.findViewById(R.id.cl_main_content), errorText, Snackbar.LENGTH_LONG);
            errorBar.show();
        }

        // TODO - add error messages for other exceptions
    }

}
