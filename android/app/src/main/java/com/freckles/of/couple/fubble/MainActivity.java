package com.freckles.of.couple.fubble;

import android.app.Activity;
import android.arch.persistence.room.Room;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.NavigationView;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v4.widget.SwipeRefreshLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;
import android.support.v7.widget.helper.ItemTouchHelper;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.freckles.of.couple.fubble.async.DeleteRoomsAsync;
import com.freckles.of.couple.fubble.async.GetRoomsAsync;
import com.freckles.of.couple.fubble.entity.FubbleRoom;
import com.freckles.of.couple.fubble.tools.AppDatabase;
import com.freckles.of.couple.fubble.tools.DatabaseHolder;
import com.freckles.of.couple.fubble.tools.JoinRoomHandler;
import com.freckles.of.couple.fubble.tools.RoomListAdapter;
import com.freckles.of.couple.fubble.tools.RoomsItemTouchHelper;
import com.freckles.of.couple.fubble.tools.WebsocketHandler;

import java.util.List;

public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener {

    private Activity activity;
    private JoinRoomHandler handler;
    private Button btnRoomJoin;
    private EditText etRoomName;
    private RecyclerView rvRooms;
    private SwipeRefreshLayout refreshLayout;

    private RoomListAdapter adapter;

    private String fubblerName;
    private TextView tvFubblerName;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        activity = this;

        handler = new JoinRoomHandler(this);

        // init views and icons
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.addDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        View headerLayout = navigationView.inflateHeaderView(R.layout.nav_header_main);
        RelativeLayout rlNavigationHeader = headerLayout.findViewById(R.id.rl_navigation_header);

        tvFubblerName = headerLayout.findViewById(R.id.tv_fubbler_name);
        etRoomName = findViewById(R.id.et_enter_room_name);
        btnRoomJoin = findViewById(R.id.btn_room_join);
        refreshLayout = findViewById(R.id.srl_rooms);
        rvRooms = findViewById(R.id.rv_rooms);


        initDatabase();
        initWebSocket();
        initFubblerName();

        // listeners
        btnRoomJoin.setOnClickListener(getRoomJoinListener());
        refreshLayout.setOnRefreshListener(getRefreshListener());
        etRoomName.addTextChangedListener(getRoomNameListener());
        rlNavigationHeader.setOnClickListener(getClickProfileListener());
        new ItemTouchHelper(new RoomsItemTouchHelper(this).create()).attachToRecyclerView(rvRooms);
    }

    private View.OnClickListener getClickProfileListener() {
        return new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent profileIntent = new Intent(activity, ProfileActivity.class);
                startActivity(profileIntent);
            }
        };
    }

    private void initWebSocket() {
        WebsocketHandler websocket = new WebsocketHandler(this);
        websocket.open();
    }

    private void initFubblerName() {
        SharedPreferences sharedPref = getSharedPreferences(getString(R.string.preference_file_key), Context.MODE_PRIVATE);
        fubblerName = sharedPref.getString(getString(R.string.saved_fubbler_name), "Fubbler");
        tvFubblerName.setText(fubblerName);
    }


    private void initDatabase() {
        DatabaseHolder.database = Room.databaseBuilder(getApplicationContext(),
                AppDatabase.class, DatabaseHolder.FUBBLE_DATABASE_NAME).build();
    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }


    @Override
    public void onResume() {
        super.onResume();
        initFubblerName();
        etRoomName.setText("");
        btnRoomJoin.setVisibility(View.GONE);
        new GetRoomsAsync(this).execute();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onNavigationItemSelected(@NonNull MenuItem item) {
        int id = item.getItemId();

        if (id == R.id.nav_settings) {
            Intent settingsIntent = new Intent(activity, SettingsActivity.class);
            startActivity(settingsIntent);
        } else if (id == R.id.nav_security) {

        } else if (id == R.id.nav_about) {

        }

        DrawerLayout drawer = findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }

    public TextWatcher getRoomNameListener() {
        return new TextWatcher() {

            public void afterTextChanged(Editable s) {
            }

            public void beforeTextChanged(CharSequence text, int start,
                                          int count, int after) {
            }

            public void onTextChanged(CharSequence text, int start,
                                      int before, int count) {
                if (text.length() > 0) {
                    btnRoomJoin.setVisibility(View.VISIBLE);
                } else {
                    btnRoomJoin.setVisibility(View.GONE);
                }
            }
        };
    }

    public View.OnClickListener getRoomJoinListener() {
        return new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                hideKeyboard();
                String roomName = etRoomName.getText().toString();
                handler.joinRoom(roomName, "");
            }
        };
    }

    private void hideKeyboard() {
        InputMethodManager imm = (InputMethodManager) activity.getSystemService(Activity.INPUT_METHOD_SERVICE);
        View view = activity.getCurrentFocus();
        if (view == null) {
            view = new View(activity);
        }
        imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
    }

    public SwipeRefreshLayout.OnRefreshListener getRefreshListener() {
        return new SwipeRefreshLayout.OnRefreshListener() {
            @Override
            public void onRefresh() {
                refreshLayout.setRefreshing(true);
                new GetRoomsAsync(activity).execute();
            }
        };
    }


    public void createRoomList(List<FubbleRoom> rooms) {
        adapter = new RoomListAdapter(rooms, activity);
        rvRooms.setAdapter(adapter);
        rvRooms.setLayoutManager(new LinearLayoutManager(activity));
        refreshLayout.setRefreshing(false);
    }

    public void deleteRoom(int position) {
        new DeleteRoomsAsync(position, adapter).execute();
    }
}
