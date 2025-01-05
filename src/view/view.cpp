#include "view/view.h"
#include "core/logger.h"
#include "view/view_group.h"
#include "application/application.h"


LOG_TAG("View");

View::View() = default;

void View::measure(int widthMeasureSpec, int heightMeasureSpec)
{
  // 默认实现：使用建议的尺寸
  int width = MeasureSpec::getSize(widthMeasureSpec);
  int height = MeasureSpec::getSize(heightMeasureSpec);
  setMeasuredDimension(width, height);
}

void View::layout(int left, int top, int right, int bottom)
{
  bounds.x = left;
  bounds.y = top;
  bounds.width = right - left;
  bounds.height = bottom - top;
  needsLayout = false;
}

void View::draw(RenderContext& context)
{
  if (!visible) {
    LOGI("View is not visible, skipping draw");
    return;
  }

  context.save();
  context.clipRect(bounds);
  onDraw(context);
  context.restore();
}

bool View::dispatchEvent(const Event& event)
{
  if (!visible)
    return false;
  return onEvent(event);
}

bool View::onEvent(const Event& event)
{
  // 基础事件处理
  return false;
}

void View::setPosition(int x, int y)
{
  if (bounds.x != x || bounds.y != y) {
    bounds.x = x;
    bounds.y = y;
    invalidate();
  }
}

void View::setSize(int width, int height)
{
  if (bounds.width != width || bounds.height != height) {
    bounds.width = width;
    bounds.height = height;
    requestLayout();
  }
}

void View::setBounds(const Rect& bounds)
{
  setPosition(bounds.x, bounds.y);
  setSize(bounds.width, bounds.height);
}

void View::setVisible(bool visible)
{
  if (this->visible != visible) {
    this->visible = visible;
    invalidate();
  }
}

void View::invalidate()
{
  // 通知 ViewRoot 需要重绘
  if (auto* wm = Application::getInstance().getWindowManager()) {
    if (auto* vr = wm->getViewRoot()) {
      vr->invalidate();
    }
  }
}

void View::requestLayout()
{
  needsLayout = true;
  if (auto* wm = Application::getInstance().getWindowManager()) {
    if (auto* vr = wm->getViewRoot()) {
      vr->requestLayout();
    }
  }
}

void View::setMeasuredDimension(int width, int height)
{
  measuredWidth = width;
  measuredHeight = height;
}

void View::setLayoutParams(const LayoutParams& params)
{
  layoutParams = params;
  requestLayout();
}

void View::setPadding(int left, int top, int right, int bottom)
{
  if (paddingLeft != left || paddingTop != top || paddingRight != right ||
      paddingBottom != bottom) {
    paddingLeft = left;
    paddingTop = top;
    paddingRight = right;
    paddingBottom = bottom;
    requestLayout();
  }
}

void View::onMeasure(int widthMeasureSpec, int heightMeasureSpec)
{
  // 默认实现：使用建议的尺寸
  int width = MeasureSpec::getSize(widthMeasureSpec);
  int height = MeasureSpec::getSize(heightMeasureSpec);
  setMeasuredDimension(width, height);
}