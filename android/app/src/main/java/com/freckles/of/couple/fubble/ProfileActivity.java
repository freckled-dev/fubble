package com.freckles.of.couple.fubble;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.text.InputType;
import android.view.MenuItem;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.TextView;

public class ProfileActivity extends AppCompatActivity {

    private String fubblerName;
    private TextView tvFubblerName;
    private SharedPreferences sharedPref;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_profile);

        getSupportActionBar().setBackgroundDrawable(new ColorDrawable(getResources().getColor(R.color.colorPrimary)));
        getSupportActionBar().setDisplayHomeAsUpEnabled(true);

        setTitle(R.string.profileHeader);

        sharedPref = getSharedPreferences(getString(R.string.preference_file_key), Context.MODE_PRIVATE);

        ImageView ivFubbler = findViewById(R.id.iv_profile_fubbler_icon);
        ImageView ivEdit = findViewById(R.id.iv_profile_edit_icon);
        ivEdit.setOnClickListener(getEditOnClickListener());
        tvFubblerName = findViewById(R.id.tv_profile_fubbler_name);
        initFubblerName();
    }


    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case android.R.id.home:
                onBackPressed();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    public View.OnClickListener getEditOnClickListener() {
        return new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                openInputNameDialog();
            }
        };
    }

    private void openInputNameDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle(getResources().getString(R.string.profile_edit_name_header));

        final EditText input = new EditText(this);
        input.setInputType(InputType.TYPE_CLASS_TEXT);
        builder.setView(input);

        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                fubblerName = input.getText().toString();
                saveName();
                tvFubblerName.setText(fubblerName);
            }
        });
        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.cancel();
            }
        });

        builder.show();
    }

    private void saveName() {
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.putString(getString(R.string.saved_fubbler_name), fubblerName);
        editor.commit();
    }

    private void initFubblerName() {
        fubblerName = sharedPref.getString(getString(R.string.saved_fubbler_name), "Fubbler");
        tvFubblerName.setText(fubblerName);
    }


}
