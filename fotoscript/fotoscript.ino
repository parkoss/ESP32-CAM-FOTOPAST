#include "esp_camera.h"
#include "FS.h"
#include "SD_MMC.h"

#define PIR_PIN 13
#define FLASH_LED 4

// Pinout pro AI-Thinker ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

void setup() {
  Serial.begin(115200);
  
  pinMode(PIR_PIN, INPUT); // Změna na čistý INPUT pro lepší kompatibilitu s PIR
  pinMode(FLASH_LED, OUTPUT);

  // Indikace zapnutí - jen jedno krátké bliknutí
  digitalWrite(FLASH_LED, HIGH); delay(100);
  digitalWrite(FLASH_LED, LOW);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Lepší rozlišení pro fotopast
  config.frame_size = FRAMESIZE_XGA; // 1024x768
  config.jpeg_quality = 10;
  config.fb_count = 1;

  // Inicializace kamery a karty
  esp_camera_init(&config);
  SD_MMC.begin("/sdcard", true); // 1-bit mód pro stabilitu

  Serial.println("Fotopast aktivni...");
}

void loop() {
  // Kontrola pohybu
  if (digitalRead(PIR_PIN) == HIGH) {
    
    // Zapnout blesk
    digitalWrite(FLASH_LED, HIGH);
    delay(100); // Čas na osvětlení scény

    // Pořídit snímek
    camera_fb_t * fb = esp_camera_fb_get();
    
    if (fb) {
      // Vytvořit unikátní název souboru
      String path = "/IMG_" + String(millis()) + ".jpg";
      
      File file = SD_MMC.open(path.c_str(), FILE_WRITE);
      if (file) {
        file.write(fb->buf, fb->len);
        file.close();
        Serial.println("Foto ulozena: " + path);
      }
      
      esp_camera_fb_return(fb);
    }
    
    // Vypnout blesk
    digitalWrite(FLASH_LED, LOW);

    // Ochranná pauza (anti-spam), aby karta nebyla hned plná
    // Nastav si podle potřeby (teď 5 sekund)
    delay(5000); 
  }
  
  delay(10); // Malá pauza pro stabilitu procesoru
}