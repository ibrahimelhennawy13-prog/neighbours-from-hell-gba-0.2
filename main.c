// مسجلات الـ GBA الأساسية للتحكم بالبكسل والشاشة
#define REG_DISPCNT  *(volatile unsigned short*)0x04000000
#define REG_KEYINPUT *(volatile unsigned short*)0x04000130

#define MEM_VRAM     ((volatile unsigned short*)0x06000000)

// إعدادات الشاشة: Mode 3 (رسم بكسل مباشر) وتفعيل الخلفية 2
#define VIDEOMODE_3  0x0003
#define BG2_ENABLE   0x0400

// أبعاد الشاشة للـ GBA
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

// أزرار التحكم
#define KEY_UP    0x0040
#define KEY_DOWN  0x0080
#define KEY_LEFT  0x0020
#define KEY_RIGHT 0x010
#define KEY_ANY   0x03FF

// الألوان الأساسية
#define COLOR_BLUE  0x7C00  // خلفية الغرفة الأزرق
#define COLOR_RED   0x001F  // البطل (المربع الأحمر)

// حجم بطل اللعبة (المربع الأحمر)
#define HERO_SIZE 10

// دالة لمسح الشاشة وتلوينها بلون موحد
void clearScreen(unsigned short color) {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        MEM_VRAM[i] = color;
    }
}

// دالة لرسم مربع (البطل) في إحداثيات معينة
void drawRect(int x, int y, int width, int height, unsigned short color) {
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            int screenX = x + i;
            int screenY = y + j;
            if (screenX >= 0 && screenX < SCREEN_WIDTH && screenY >= 0 && screenY < SCREEN_HEIGHT) {
                MEM_VRAM[screenY * SCREEN_WIDTH + screenX] = color;
            }
        }
    }
}

// دالة لمزامنة الإطارات مع الشاشة لمنع الوميض (VBlank)
void vsync() {
    volatile unsigned short* scanline = (volatile unsigned short*)0x04000006;
    while (*scanline >= 160);
    while (*scanline < 160);
}

int main() {
    // 1. تفعيل مود الشاشة الرسمية للـ GBA
    REG_DISPCNT = VIDEOMODE_3 | BG2_ENABLE;

    // إحداثيات البطل الأولية (في المنتصف)
    int heroX = (SCREEN_WIDTH - HERO_SIZE) / 2;
    int heroY = (SCREEN_HEIGHT - HERO_SIZE) / 2;

    // إحداثيات البطل السابقة لمسح أثره عند الحركة
    int prevX = heroX;
    int prevY = heroY;

    // تلوين الخلفية بالأزرق لأول مرة
    clearScreen(COLOR_BLUE);

    // رسم البطل في البداية
    drawRect(heroX, heroY, HERO_SIZE, HERO_SIZE, COLOR_RED);

    // حلقة اللعبة اللانهائية
    while (1) {
        vsync(); // الانتظار حتى تحديث الشاشة

        // قراءة حالة الأزرار (في الـ GBA الصفر يعني مضغوط)
        unsigned short keys = ~REG_KEYINPUT;

        // تحديث الاتجاهات مع الحفاظ على حدود الشاشة
        if (keys & KEY_UP) {
            if (heroY > 0) heroY--;
        }
        if (keys & KEY_DOWN) {
            if (heroY < SCREEN_HEIGHT - HERO_SIZE) heroY++;
        }
        if (keys & KEY_LEFT) {
            if (heroX > 0) heroX--;
        }
        if (keys & KEY_RIGHT) {
            if (heroX < SCREEN_WIDTH - HERO_SIZE) heroX++;
        }

        // إذا تحرك البطل، نمسح مكانه القديم ونرسمه في الجديد
        if (heroX != prevX || heroY != prevY) {
            // مسح الأثر القديم بتلوينه بلون الخلفية (الأزرق)
            drawRect(prevX, prevY, HERO_SIZE, HERO_SIZE, COLOR_BLUE);
            // رسم البطل في مكانه الجديد (الأحمر)
            drawRect(heroX, heroY, HERO_SIZE, HERO_SIZE, COLOR_RED);

            // حفظ الإحداثيات الحالية لتصبح سابقة في الإطار القادم
            prevX = heroX;
            prevY = heroY;
        }
    }

    return 0;
}
