#include "video_layout.hpp"
#include <GuillotineBinPack.h>
#include <QDebug>
#include <cassert>
#include <cmath>
#include <iostream>
#include <optional>
#include <utility>

#define ENABLE_LOGGING 0

namespace {
bool has_property_aspect(QQuickItem &check) {
  return check.property("aspect").isValid();
}
std::pair<int, int> calculate_width_and_hight_by_aspect(double aspect) {
  const int area{10000};
  const int x = std::sqrt(area * aspect);
  const int y = static_cast<int>(static_cast<double>(x) / aspect);
  return std::make_pair(x, y);
}
double get_aspect(QQuickItem &item) {
  return item.property("aspect").toDouble();
}
using rects_type = std::vector<rbp::Rect>;
std::optional<rects_type> try_pack(const QList<QQuickItem *> &children_,
                                   const double container_aspect,
                                   const int packer_width) {
  const auto packer_height = static_cast<int>(packer_width / container_aspect);
#if ENABLE_LOGGING
  qDebug() << "packer_width:" << packer_width
           << ", packer_height:" << packer_height;
#endif
  rbp::GuillotineBinPack packer{static_cast<int>(packer_width), packer_height,
                                false};
  const bool merge{true};
  const auto choice = rbp::GuillotineBinPack::RectWorstShortSideFit;
  const auto split = rbp::GuillotineBinPack::SplitMinimizeArea;
  rects_type results;
#if 0
  int fun{};
#endif
  for (const auto &child : children_) {
    auto child_aspect = get_aspect(*child);
#if ENABLE_LOGGING
    qDebug() << "child_aspect:" << child_aspect;
#endif
#if 0
    if (fun == 1)
      child_aspect = 1.0 / child_aspect;
    ++fun;
#endif
    auto [child_width, child_height] =
        calculate_width_and_hight_by_aspect(child_aspect);
    auto result =
        packer.Insert(child_width, child_height, merge, choice, split);
    results.push_back(result);
    if (result.width == 0 || result.height == 0) {
#if ENABLE_LOGGING
      qDebug() << "has no width||height";
#endif
      return std::nullopt;
    }
  }
  return results;
}
void set_rects_to_qml(const rects_type &rects,
                      const QList<QQuickItem *> &children, const double factor,
                      const int packer_width, const int packer_height,
                      const qreal offset) {
  assert(static_cast<std::size_t>(children.size()) == rects.size());
  const int max_x = std::accumulate(
      rects.cbegin(), rects.cend(), 0, [](int current, rbp::Rect check) {
        return std::max(current, check.x + check.width);
      });
  double offset_x = ((packer_width - max_x) * factor) / 2.0;
  const int max_y = std::accumulate(
      rects.cbegin(), rects.cend(), 0, [](int current, rbp::Rect check) {
        return std::max(current, check.y + check.height);
      });
  double offset_y = ((packer_height - max_y) * factor) / 2.0 + offset;
  for (int index{}; index < children.size(); ++index) {
    auto rect = rects[index];
    double x = static_cast<double>(rect.x) * factor + offset_x;
    double y = static_cast<double>(rect.y) * factor + offset_y;
    double width = static_cast<double>(rect.width) * factor;
    double height = static_cast<double>(rect.height) * factor;
    auto child = children[index];
    child->setX(x);
    child->setY(y);
    child->setWidth(width);
    child->setHeight(height);
  }
}
void calculate(const QList<QQuickItem *> &items, qreal width_, qreal height_,
               qreal offset) {
  const double container_aspect = width_ / height_;
  const double first_child_aspect = get_aspect(*items.first());
  if (container_aspect <= 0. || first_child_aspect <= 0.)
    return;
  const auto [first_child_width, first_child_height] =
      calculate_width_and_hight_by_aspect(first_child_aspect);
  auto packer_width = static_cast<int>(first_child_width);
  while (true) {
    auto packed = try_pack(items, container_aspect, packer_width);
    if (!packed) {
      auto packer_width_old = packer_width;
      packer_width += (packer_width * 5) / 100;
      if (packer_width_old >= packer_width)
        return; // this is going nowhere. there's an invalid number involved
      continue;
    }
    const double container_packer_factor =
        width_ / static_cast<double>(packer_width);
    const auto packer_height =
        static_cast<int>(packer_width / container_aspect);
    set_rects_to_qml(packed.value(), items, container_packer_factor,
                     packer_width, packer_height, offset);
    break;
  }
}
} // namespace

video_layout::video_layout(QQuickItem *parent) : QQuickItem(parent) {
#if ENABLE_LOGGING
  qDebug() << "video_layout got instanced";
#endif
  connect(this, &QQuickItem::heightChanged, this, [this] { recalculate(); });
  connect(this, &QQuickItem::widthChanged, this, [this] { recalculate(); });
}

void video_layout::itemChange(QQuickItem::ItemChange change,
                              const QQuickItem::ItemChangeData &value) {
#if ENABLE_LOGGING
  qDebug() << "change:" << change << ", width:" << width();
#endif
  if (change == QQuickItem::ItemChange::ItemChildAddedChange)
    return on_child_added(value);
  if (change == QQuickItem::ItemChange::ItemChildRemovedChange) {
    if (focused == value.item)
      clear_focused();
    return recalculate();
  }
}

void video_layout::clear_focused() {
  set_layout_property(layout::grid);
  focused = nullptr;
}

void video_layout::on_child_added(const QQuickItem::ItemChangeData &value) {
  if (!has_property_aspect(*value.item))
    return;
  connect(value.item, SIGNAL(aspectChanged()), this, SLOT(on_aspect_changed()));
  recalculate();
}

void video_layout::on_aspect_changed() {
#if ENABLE_LOGGING
  qDebug() << "on_aspect_changed()";
#endif
  recalculate();
}

void video_layout::recalculate() {
  // TODO save width/height/count - and don't recalculate
  auto width_ = width();
  auto height_ = height();
  auto children_all = childItems();
  QList<QQuickItem *> children_;
  std::copy_if(children_all.begin(), children_all.end(),
               std::back_inserter(children_),
               [&](auto check) { return has_property_aspect(*check); });
  if (width_ == 0 || height_ == 0 || children_.empty())
    return;
#if ENABLE_LOGGING
  qDebug() << "recalculate, width_:" << width_ << ", height_:" << height_
           << "children_.size():" << children_.size();
#endif
  const layout actual_layout = [&] {
    if (children_.size() == 1)
      return layout::grid;
    return layout_;
  }();
  if (actual_layout != layout::full)
    for (auto show : children_)
      show->setVisible(true);
  if (actual_layout == layout::grid)
    return calculate(children_, width_, height_, 0.);
  QList<QQuickItem *> without_focused;
  std::remove_copy(children_.cbegin(), children_.cend(),
                   std::back_inserter(without_focused), focused);
  assert(!without_focused.empty());
  if (actual_layout == layout::full) {
    assert(focused);
    children_.clear();
    children_.push_back(focused);
    focused->setVisible(true);
    for (auto remove : without_focused)
      remove->setVisible(false);
    return calculate(children_, width_, height_, 0.);
  }
  if (actual_layout == layout::enlarged) {
    qreal smalls_height = (height_ * 20) / 100;
    calculate(without_focused, width_, smalls_height, 0.);
    QList<QQuickItem *> focused_list;
    focused_list.push_back(focused);
    return calculate(focused_list, width_, height_ - smalls_height,
                     smalls_height);
  }
  assert(false && "must not be reached");
}

void video_layout::enlarge(QQuickItem *item) {
#if ENABLE_LOGGING
  qDebug() << __FUNCTION__ << item;
#endif
  assert(item != nullptr);
  if (focused == item || layout_ == layout::grid) {
    // lets enlarge!
    focused = item;
    switch (layout_) {
    case layout::grid:
      set_layout_property(layout::enlarged);
      break;
    case layout::enlarged:
      set_layout_property(layout::full);
      break;
    case layout::full:
      set_layout_property(layout::grid);
      break;
    }
  } else {
    // let's only switch enlarged target
    focused = item;
    recalculate();
  }
}

void video_layout::shrink([[maybe_unused]] QQuickItem *item) {
#if ENABLE_LOGGING
  qDebug() << __FUNCTION__ << item;
#endif
  assert(item != nullptr);
  switch (layout_) {
  case layout::grid:
    return;
  case layout::enlarged:
    set_layout_property(layout::grid);
    break;
  case layout::full:
    set_layout_property(layout::full);
    break;
  }
}

void video_layout::set_layout_property(layout set) {
  if (set == layout_)
    return;
  layout_ = set;
  layout_changed(get_layout());
  recalculate();
}

QString video_layout::get_layout() const { return layout_to_string(layout_); }

void video_layout::set_layout(QString set) {
  const auto casted = [&] {
    if (set == "enlarged")
      return layout::enlarged;
    if (set == "full")
      return layout::full;
    assert(set == "grid");
    return layout::grid;
  }();
  set_layout_property(casted);
}

QString video_layout::layout_to_string(layout cast) const {
  switch (cast) {
  case layout::grid:
    return "grid";
  case layout::enlarged:
    return "enlarged";
  case layout::full:
    return "full";
  }
  assert(false);
  return "grid";
}
