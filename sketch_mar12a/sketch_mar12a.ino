#include<FS.h>
#include<SPIFFS.h>
#include<Wire.h>
#include<SSD1306Wire.h>

const int I2C_clock = 340000000;
const int buf_size = 84 * 64 * 2 + 10;

char buf[buf_size];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(3000);
  Serial.println("Opening SPIFFS");
  SPIFFS.begin(true);
  Serial.println("SPIFFS ready");
  //  Serial.println(SPIFFS.format());

  if (!SPIFFS.exists("/bad_apple_full86.codeb")) {
    Serial.println("Cannot find file");
  }

  SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_64, I2C_ONE, I2C_clock);
  display.init();
  display.displayOn();
  display.setColor(INVERSE);

  File file = SPIFFS.open("/bad_apple_full86.codeb", FILE_READ);
  if (!file) {
    Serial.println("Open file fail");
    return;
  }

  bool fg = true;
  while (fg) {
    file.readBytes(buf, buf_size);
    int cnt = 0;
    unsigned long last = micros();
    while (true) {
      int16_t x = buf[cnt++];
      if (cnt == buf_size) {
        file.readBytes(buf, buf_size);
        cnt = 0;
      }
      if (x >= 128) {
        x &= 127;
        int16_t y = buf[cnt++];
        if (cnt == buf_size) {
          file.readBytes(buf, buf_size);
          cnt = 0;
        }
        y &= 127;
        display.setPixel(x, y);
      }
      else if (x == 0x01) {
        while (micros() - last <= 33333);
        last = micros();
        display.display();

      }
      else {
        fg = false;
        break;
      }
    }
  }

  SPIFFS.end();
}

void loop() {
  // put your main code here, to run repeatedly:

}
