#include <Arduino.h>
#include <BLEDevice.h>
#include <BleKeyboard.h>
#include <ESP32Encoder.h>

// Pin definitions
#define CLK 32           // Encoder CLK pin
#define DT 14            // Encoder DT pin
#define BTN_PLAY 16      // Button for Play/Pause
#define BTN_NEXT 4      // Button for Next Track
#define BTN_PREV 17      // Button for Previous Track
#define BTN_MUTE 5      // Button for Previous Track
#define BTN_SHORTCUT1 33 // Button for Shortcut 1 (e.g., Ctrl+C)
#define BTN_SHORTCUT2 25 // Button for Shortcut 2 (e.g., Ctrl+V)
#define BTN_SHORTCUT3 27 // Button for Shortcut 3 (e.g., Ctrl+V)

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
  pinMode(BTN_SHORTCUT2, INPUT_PULLUP);
  pinMode(BTN_SHORTCUT3, INPUT_PULLUP);
}

void loop()
{
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
