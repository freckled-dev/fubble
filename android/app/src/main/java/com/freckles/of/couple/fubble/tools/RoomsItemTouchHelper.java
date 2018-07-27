package com.freckles.of.couple.fubble.tools;

import android.app.Activity;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.support.v4.content.ContextCompat;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.helper.ItemTouchHelper;
import android.view.View;

import com.freckles.of.couple.fubble.MainActivity;
import com.freckles.of.couple.fubble.R;

public class RoomsItemTouchHelper {

    private static final int MARGIN_ITEM_ROW = 20;

    private Drawable deleteIcon;
    private Paint paint;
    private MainActivity activity;

    public RoomsItemTouchHelper(Activity activity) {
        this.activity = (MainActivity) activity;

        initDeleteIcon();
        initPaint();
    }

    private void initDeleteIcon() {
        deleteIcon = activity.getDrawable(R.drawable.ic_delete);
    }

    private void initPaint() {
        paint = new Paint();
        int color = ContextCompat.getColor(activity, R.color.colorRed);
        paint.setColor(color);
    }

    public ItemTouchHelper.SimpleCallback create() {
        return new ItemTouchHelper.SimpleCallback(0, ItemTouchHelper.RIGHT) {

            @Override
            public boolean onMove(RecyclerView recyclerView, RecyclerView.ViewHolder viewHolder, RecyclerView.ViewHolder target) {
                return false;
            }

            @Override
            public void onSwiped(RecyclerView.ViewHolder viewHolder, int direction) {
                activity.deleteRoom(viewHolder.getAdapterPosition());
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

}
