package com.freckles.of.couple.fubble;

import android.app.Activity;
import android.arch.persistence.room.Room;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.design.widget.NavigationView;
import android.support.v4.content.ContextCompat;
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

import com.freckles.of.couple.fubble.entity.FubbleRoom;
import com.freckles.of.couple.fubble.tools.AppDatabase;
import com.freckles.of.couple.fubble.tools.DatabaseHolder;
import com.freckles.of.couple.fubble.tools.JoinRoomHandler;
import com.freckles.of.couple.fubble.tools.RoomListAdapter;
import com.freckles.of.couple.fubble.tools.WebsocketHandler;

import java.util.Collections;
import java.util.Comparator;
import java.util.List;

public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener {

    public static final int MARGIN_ITEM_ROW = 20;
    
    private Paint paint;
    private Activity activity;
    private JoinRoomHandler handler;
    private Button btnRoomJoin;
    private EditText etRoomName;
    private RecyclerView rvRooms;
    private SwipeRefreshLayout refreshLayout;
    private Drawable deleteIcon;

    private RoomListAdapter adapter;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        activity = this;

        handler = new JoinRoomHandler(this);

        initDatabase();
        initPaint();

        WebsocketHandler websocket = new WebsocketHandler(this);
        websocket.open();

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

        deleteIcon = activity.getDrawable(R.drawable.ic_delete);

        etRoomName = findViewById(R.id.et_enter_room_name);
        etRoomName.addTextChangedListener(getRoomNameListener());

        btnRoomJoin = findViewById(R.id.btn_room_join);
        btnRoomJoin.setOnClickListener(getRoomJoinListener());

        refreshLayout = findViewById(R.id.srl_rooms);
        refreshLayout.setOnRefreshListener(getRefreshListener());

        rvRooms = findViewById(R.id.rv_rooms);
        new ItemTouchHelper(getItemTouchHelperCallback()).attachToRecyclerView(rvRooms);
    }

    private void initPaint() {
        paint = new Paint();
        int color = ContextCompat.getColor(activity, R.color.colorRed);
        paint.setColor(color);
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

        if (id == R.id.nav_gallery) {

        } else if (id == R.id.nav_slideshow) {

        } else if (id == R.id.nav_manage) {

        } else if (id == R.id.nav_share) {

        } else if (id == R.id.nav_send) {

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
        Activity activity = this;
        InputMethodManager imm = (InputMethodManager) activity.getSystemService(Activity.INPUT_METHOD_SERVICE);
        //Find the currently focused view, so we can grab the correct window token from it.
        View view = activity.getCurrentFocus();
        //If no view currently has focus, create a new one, just so we can grab a window token from it
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

    public ItemTouchHelper.SimpleCallback getItemTouchHelperCallback() {
        return new ItemTouchHelper.SimpleCallback(0, ItemTouchHelper.RIGHT) {

            @Override
            public boolean onMove(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder, RecyclerView.ViewHolder target) {
                return false;
            }

            @Override
            public void onSwiped(RecyclerView.ViewHolder viewHolder, int direction) {
                adapter.onItemDismiss(viewHolder.getAdapterPosition());
            }

            @Override
            public void onChildDraw(Canvas canvas, RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder, float dX, float dY, int actionState, boolean isCurrentlyActive) {
                if (actionState == ItemTouchHelper.ACTION_STATE_SWIPE) {

                    View itemView = viewHolder.itemView;

                    if (dX > 0) {
                        drawBackground(canvas, dX, itemView);
                        drawIcon(canvas, dX, itemView);
                    }

                    super.onChildDraw(canvas, recyclerView, viewHolder, dX, dY, actionState, isCurrentlyActive);
                }
            }

            private void drawIcon(Canvas canvas, float dX, View itemView) {
                int iconWidth = deleteIcon.getIntrinsicWidth();
                int iconHeight = deleteIcon.getIntrinsicWidth();
                int rowHeight = itemView.getBottom() - itemView.getTop();
                int iconMargin = (rowHeight - iconHeight) / 2;

                int iconLeft = iconMargin;
                int iconRight = iconWidth + iconMargin;
                int iconTop = itemView.getTop() + (rowHeight - iconHeight) / 2;
                int iconBottom = iconTop + iconHeight;

                if (dX > iconRight + MARGIN_ITEM_ROW) {
                    Rect iconDest = new Rect(iconLeft, iconTop, iconRight, iconBottom);
                    deleteIcon.setBounds(iconDest);
                    deleteIcon.draw(canvas);
                }
            }

            private void drawBackground(Canvas canvas, float dX, View itemView) {
                RectF background = new RectF((float) itemView.getLeft(), (float) itemView.getTop(), dX - MARGIN_ITEM_ROW, (float) itemView.getBottom());
                canvas.drawRect(background, paint);
            }

            @Override
            public void clearView(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder) {
                super.clearView(recyclerView, viewHolder);
            }
        };
    }


    private class GetRoomsAsync extends AsyncTask<Void, Void, List<FubbleRoom>> {

        private Activity activity;

        public GetRoomsAsync(Activity activity) {
            this.activity = activity;
        }

        @Override
        protected List<FubbleRoom> doInBackground(Void... params) {
            return DatabaseHolder.database.roomDao().getAll();
        }

        @Override
        protected void onPostExecute(final List<FubbleRoom> rooms) {
            Collections.sort(rooms, new Comparator<FubbleRoom>() {
                @Override
                public int compare(FubbleRoom r1, FubbleRoom r2) {
                    return r2.getLastConnected().compareTo(r1.getLastConnected());
                }
            });

            adapter = new RoomListAdapter(rooms, activity);
            rvRooms.setAdapter(adapter);

            rvRooms.setLayoutManager(new LinearLayoutManager(activity));

            refreshLayout.setRefreshing(false);
        }
    }
}
