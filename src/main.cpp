#include <Arduino.h>
#include <BLEDevice.h>
#include <BleKeyboard.h>
#include <ESP32Encoder.h>

// Pin definitions
#define CLK 32           // Encoder CLK pin
#define DT 14            // Encoder DT pin
#define BTN_PLAY 16      // Button for Play/Pause
#define BTN_NEXT 4       // Button for Next Track
#define BTN_PREV 17      // Button for Previous Track
#define BTN_MUTE 5       // Button for Mute
#define BTN_SHORTCUT1 33 // Button for Shortcut 1
#define BTN_SHORTCUT2 25 // Button for Shortcut 2 (Wake-up source)
#define BTN_SHORTCUT3 27 // Button for Shortcut 3

ESP32Encoder encoder;
BleKeyboard bleKeyboard;

void setup()
{
  // Initialize encoder
  encoder.attachHalfQuad(DT, CLK);
  encoder.setCount(0);

  // Initialize BLE Keyboard
  BLEDevice::init("XTR7");
  bleKeyboard.begin();

  // Initialize serial communication
  Serial.begin(115200);

  // Initialize button pins
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_MUTE, INPUT_PULLUP);
  pinMode(BTN_SHORTCUT1, INPUT_PULLUP);
  pinMode(BTN_SHORTCUT2, INPUT); // Use internal pull-up for wake-up
  pinMode(BTN_SHORTCUT3, INPUT_PULLUP);
  // Check if waking from deep sleep
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1)
  {
    Serial.println("Woke up from deep sleep due to EXT1 (falling edge).");
  }
  else
  {
    Serial.println("Woke up from deep sleep due to other reason.");
  }

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT1)
  {
    Serial.println("Woke up from deep sleep!");
  }
}

void loop()
{
  // Handle deep sleep activation
  if (digitalRead(BTN_PLAY) == LOW && digitalRead(BTN_SHORTCUT2) == LOW)
  {
    Serial.println("Entering deep sleep...");
    delay(200); // Debounce delay
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    // Configure BTN_SHORTCUT2 as the wake-up source for falling edge (LOW state)
    esp_sleep_enable_ext1_wakeup((1ULL << BTN_SHORTCUT2), ESP_EXT1_WAKEUP_ALL_LOW);

    esp_deep_sleep_start();
  }
  if (digitalRead(BTN_NEXT) == LOW && digitalRead(BTN_SHORTCUT3) == LOW)
  {
    Serial.println("Resetting ESP32...");
    delay(500);    // Short delay to confirm button press
    esp_restart(); // Reset the ESP32
  }
  // Check if BLE keyboard is connected
  if (bleKeyboard.isConnected())
  {
    // Handle rotary encoder actions
    long newPosition = encoder.getCount() / 2; // Read encoder position

    if (newPosition > 0)
    {
      bleKeyboard.write(KEY_MEDIA_VOLUME_UP); // Volume up
      encoder.setCount(0);                    // Reset encoder count
    }
    else if (newPosition < 0)
    {
      bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN); // Volume down
      encoder.setCount(0);                      // Reset encoder count
    }

    // Handle button presses for media actions
    if (digitalRead(BTN_PLAY) == LOW)
    {
      bleKeyboard.write(KEY_MEDIA_PLAY_PAUSE); // Play/Pause
      delay(200);                              // Debounce delay
    }

    if (digitalRead(BTN_NEXT) == LOW)
    {
      bleKeyboard.write(KEY_MEDIA_NEXT_TRACK); // Next Track
      delay(200);                              // Debounce delay
    }

    if (digitalRead(BTN_PREV) == LOW)
    {
      bleKeyboard.write(KEY_MEDIA_PREVIOUS_TRACK); // Previous Track
      delay(200);                                  // Debounce delay
    }

    if (digitalRead(BTN_MUTE) == LOW)
    {
      bleKeyboard.write(KEY_MEDIA_MUTE);
      delay(200); // Debounce delay
    }

    // Handle button presses for shortcuts
    if (digitalRead(BTN_SHORTCUT1) == LOW)
    {
      bleKeyboard.press(KEY_LEFT_GUI);
      bleKeyboard.press('1');
      bleKeyboard.releaseAll(); // Release all keys
      delay(200);               // Debounce delay
    }

    if (digitalRead(BTN_SHORTCUT2) == LOW)
    {
      // Send Ctrl+V shortcut (Paste)
      bleKeyboard.press(KEY_LEFT_GUI);
      bleKeyboard.press('2');
      bleKeyboard.releaseAll(); // Release all keys
      delay(200);               // Debounce delay
    }
    if (digitalRead(BTN_SHORTCUT3) == LOW)
    {
      bleKeyboard.press(KEY_LEFT_ALT);   // Press Alt key
      bleKeyboard.press(KEY_LEFT_SHIFT); // Press Shift key
      bleKeyboard.press(KEY_F11);        // Press F11 key
      delay(100);                        // Small delay to simulate keypress duration
      bleKeyboard.releaseAll();          // Release all keys
      delay(200);                        // Debounce delay
    }
  }

  delay(50); // Main loop delay
}
