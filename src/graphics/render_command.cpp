#include "graphics/render_command.h"

void ClearCommand::execute(RenderContext& context) {
    context.clear(color);
}

void DrawRectCommand::execute(RenderContext& context) {
    context.drawRect(rect, paint);
}

void DrawTextCommand::execute(RenderContext& context) {
    context.drawText(text, x, y, paint);
}

// 添加更多渲染命令的实现... 