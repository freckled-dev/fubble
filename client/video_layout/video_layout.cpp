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
  int fun{};
  for (const auto &child : children_) {
    auto child_aspect = get_aspect(*child);
#if ENABLE_LOGGING
    qDebug() << "child_aspect:" << child_aspect;
#endif
#if 0
    if (fun == 1)
      child_aspect = 1.0 / child_aspect;
#endif
    ++fun;
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
                      const int packer_width, const int packer_height) {
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
  double offset_y = ((packer_height - max_y) * factor) / 2.0;
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
  if (change == QQuickItem::ItemChange::ItemChildRemovedChange)
    return recalculate();
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
  calculate(children_all, width_, height_);
}

void video_layout::enlarge(QQuickItem *item) {
  qDebug() << __FUNCTION__ << item;
}

void video_layout::shrink(QQuickItem *item) {
  qDebug() << __FUNCTION__ << item;
}

QString video_layout::get_layout() const {
  switch (layout_) {
  case layout::grid:
    return "grid";
  case layout::enlarged:
    return "enlarged";
  case layout::full:
    return "full";
  }
  assert(false);
}

void video_layout::set_layout(QString set) {
  const auto casted = [&] {
    if (set == "enlarged")
      return layout::enlarged;
    if (set == "full")
      return layout::full;
    assert(set == "grid");
    return layout::grid;
  }();
  if (layout_ == casted)
    return;
  layout_ = casted;
  layout_changed(set);
}

void video_layout::calculate(const QList<QQuickItem *> &items, qreal width_,
                             qreal height_) {
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
                     packer_width, packer_height);
    break;
  }
}
