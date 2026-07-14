// كود تجريبي للعبة إزاي تخنق جارك على الـ GBA
// المربع الأحمر يمثل "ودي" والتحريك بأزرار الاتجاهات

#define REG_DISPCNT  *(volatile unsigned short*)0x04000000
#define MEM_VRAM     0x06000000
#define VIDEOMODE_3  0x0003
#define BG2_ENABLE   0x0400

#define REG_KEYINPUT *(volatile unsigned short*)0x04000130

#define KEY_UP    64
#define KEY_DOWN  128
#define KEY_LEFT  32
#define KEY_RIGHT 16

unsigned short* videoBuffer = (unsigned short*)MEM_VRAM;

// دالة لرسم مربع ملون على الشاشة
void drawRect(int x, int y, int width, int height, unsigned short color) {
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int currentX = x + col;
            int currentY = y + row;
            if (currentX >= 0 && currentX < 240 && currentY >= 0 && currentY < 160) {
                videoBuffer[currentY * 240 + currentX] = color;
            }
        }
    }
}

// دالة لتركيب الألوان
unsigned short makeColor(unsigned char red, unsigned char green, unsigned char blue) {
    return (red & 0x1F) | ((green & 0x1F) << 5) | ((blue & 0x1F) << 10);
}

int main() {
    // تشغيل نظام الشاشة Mode 3 لرسوم الـ 2D المباشرة
    REG_DISPCNT = VIDEOMODE_3 | BG2_ENABLE;

    // ألوان اللعبة الأساسية
    unsigned short blueColor = makeColor(10, 15, 31);   // لون الغرفة (خلفية زرقاء)
    unsigned short redColor = makeColor(31, 5, 5);     // لون البطل "ودي" (مربع أحمر)

    // إحداثيات البطل ودي في البداية (في منتصف الشاشة)
    int woodyX = 110;
    int woodyY = 75;
    int woodySize = 15;

    while (1) {
        // 1. مسح الشاشة بالكامل باللون الأزرق لتهيئة الغرفة
        for (int i = 0; i < 240 * 160; i++) {
            videoBuffer[i] = blueColor;
        }

        // 2. قراءة أزرار جهاز الـ X9 وتحريك "ودي"
        unsigned short keys = ~REG_KEYINPUT;

        if (keys & KEY_RIGHT) {
            woodyX += 2;
            if (woodyX > 240 - woodySize) woodyX = 240 - woodySize;
        }
        if (keys & KEY_LEFT) {
            woodyX -= 2;
            if (woodyX < 0) woodyX = 0;
        }
        if (keys & KEY_UP) {
            woodyY -= 2;
            if (woodyY < 0) woodyY = 0;
        }
        if (keys & KEY_DOWN) {
            woodyY += 2;
            if (woodyY > 160 - woodySize) woodyY = 160 - woodySize;
        }

        // 3. رسم البطل "ودي" في مكانه الجديد
        drawRect(woodyX, woodyY, woodySize, woodySize, redColor);

        // انتظار بسيط جداً لضبط سرعة اللعبة
        for (volatile int delay = 0; delay < 1500; delay++);
    }

    return 0;
}
