#include "image_viewer.h"
#include <vector>

int main() {
    ImageViewer viewer(L"Test Image", 400, 300);
    
    // 创建一个红色渐变的测试图像
    std::vector<uint8_t> imageData(400 * 300 * 4);
    for (int y = 0; y < 300; ++y) {
        for (int x = 0; x < 400; ++x) {
            int index = (y * 400 + x) * 4;
            imageData[index + 0] = static_cast<uint8_t>(x * 255 / 400);  // B
            imageData[index + 1] = 0;                                     // G
            imageData[index + 2] = 0;                                     // R
            imageData[index + 3] = 255;                                   // A
        }
    }
    
    viewer.ShowImage(imageData.data(), 400, 300);
    
    while (viewer.ProcessMessages()) {
        Sleep(10);
    }
    
    return 0;
} 