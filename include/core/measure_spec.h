#pragma once

class MeasureSpec {
public:
    enum Mode {
        UNSPECIFIED = 0,
        EXACTLY = 1,
        AT_MOST = 2
    };
    
    static int makeMeasureSpec(int size, Mode mode);
    static Mode getMode(int measureSpec);
    static int getSize(int measureSpec);
    
private:
    static const int MODE_SHIFT = 30;
    static const int MODE_MASK = 0x3 << MODE_SHIFT;
}; 