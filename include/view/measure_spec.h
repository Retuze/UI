#pragma once
#include <algorithm>
class MeasureSpec {
public:
    enum Mode {
        UNSPECIFIED = 0,
        EXACTLY = 1,
        AT_MOST = 2
    };
    
    static int makeMeasureSpec(int size, Mode mode) {
        return (size & ~MODE_MASK) | (mode & MODE_MASK);
    }
    
    static int getMode(int measureSpec) {
        return (measureSpec & MODE_MASK);
    }
    
    static int getSize(int measureSpec) {
        return (measureSpec & ~MODE_MASK);
    }
    
    static int resolveSize(int size, int measureSpec) {
        int specMode = getMode(measureSpec);
        int specSize = getSize(measureSpec);
        
        switch (specMode) {
            case EXACTLY:
                return specSize;
            case AT_MOST:
                return std::min(size, specSize);
            case UNSPECIFIED:
            default:
                return size;
        }
    }
    
private:
    static const int MODE_SHIFT = 30;
    static const int MODE_MASK = 0x3 << MODE_SHIFT;
}; 