#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// I2C address
#define OLED_ADDR 0x3C
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi credentials - open network
const char* ssid = "RomanticGift";
const char* password = "";

WebServer server(80);

// Animation states
enum AnimationState {
  HEART_ANIMATION,
  EYES_ANIMATION,
  LOVE_STORY_ANIMATION,
  MESSAGE_DISPLAY
};
AnimationState currentState = LOVE_STORY_ANIMATION;

// Romantic messages
const char* romanticMessages[] = {
  "I love you!",
  "You complete me",
  "My heart is yours",
  "Forever with you",
  "You are my sunshine",
  "Thinking of you",
  "My love for you grows daily"
};
int messageIndex = 0;

// Animation frames for the romantic story
const int frameCount = 12;
int currentFrame = 0;
unsigned long lastFrameTime = 0;
const int frameDelay = 1000; // ms between frames

// Animation timing
unsigned long lastStateChange = 0;
const int stateDuration = 8000; // ms per animation state

// Function declarations
void displayMessage(const char* message, bool scroll = false);
void drawHeart();
void drawSmallHeart(int x, int y);
void drawEyes(int pupilX, int pupilY);
void drawRomanticStory(int frame);
void drawPerson(int x, int y, int direction);
void drawElderlyPerson(int x, int y, int direction);
void drawKneelingPerson(int x, int y);
void drawEmbracingPeople(int x, int y);

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C communication with the OLED
  Wire.begin(21, 22);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {} // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS initialization failed!");
    return;
  }
  
  // Create WiFi access point (open network)
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Initialize web server
  server.on("/", HTTP_GET, handleRoot);
  server.on("/play", HTTP_GET, handlePlay);
  server.on("/pause", HTTP_GET, handlePause);
  server.on("/next", HTTP_GET, handleNext);
  server.on("/previous", HTTP_GET, handlePrevious);
  server.on("/story", HTTP_GET, handleStory);
  server.on("/hearts", HTTP_GET, handleHearts);
  server.on("/eyes", HTTP_GET, handleEyes);
  server.on("/messages", HTTP_GET, handleMessages);
  server.begin();
  
  // Show welcome message
  displayMessage("Connect to WiFi: RomanticGift Then visit: 192.168.4.1", true);
  delay(5000);
  displayMessage("Our Love Story");
  delay(2000);
}

void loop() {
  server.handleClient();
  
  // Change animation state periodically if not manually controlled
  if (millis() - lastStateChange > stateDuration) {
    switch (currentState) {
      case HEART_ANIMATION:
        currentState = EYES_ANIMATION;
        break;
      case EYES_ANIMATION:
        currentState = LOVE_STORY_ANIMATION;
        break;
      case LOVE_STORY_ANIMATION:
        currentState = MESSAGE_DISPLAY;
        break;
      case MESSAGE_DISPLAY:
        currentState = HEART_ANIMATION;
        // Move to next message
        messageIndex = (messageIndex + 1) % (sizeof(romanticMessages) / sizeof(romanticMessages[0]));
        break;
    }
    lastStateChange = millis();
  }
  
  // Run the current animation
  switch (currentState) {
    case HEART_ANIMATION:
      drawHeart();
      break;
    case EYES_ANIMATION:
      drawEyes(random(-5, 6), random(-5, 6));
      break;
    case LOVE_STORY_ANIMATION:
      if (millis() - lastFrameTime > frameDelay) {
        drawRomanticStory(currentFrame);
        currentFrame = (currentFrame + 1) % frameCount;
        lastFrameTime = millis();
      }
      break;
    case MESSAGE_DISPLAY:
      displayMessage(romanticMessages[messageIndex]);
      break;
  }
  
  delay(100);
}

void displayMessage(const char* message, bool scroll) {
  display.clearDisplay();
  display.setCursor(0, 28);
  display.setTextSize(1);
  
  if (scroll) {
    // Simple scrolling text implementation
    static int scrollPosition = 0;
    static unsigned long lastScroll = 0;
    
    if (millis() - lastScroll > 150) {
      scrollPosition++;
      lastScroll = millis();
    }
    
    String displayText = message;
    if (scrollPosition > displayText.length() * 6 + SCREEN_WIDTH) {
      scrollPosition = 0;
    }
    
    display.setCursor(SCREEN_WIDTH - scrollPosition, 28);
    display.println(displayText);
  } else {
    // Center the message
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 28);
    display.println(message);
  }
  
  display.display();
}

void drawHeart() {
  display.clearDisplay();
  // Draw a simple heart shape
  display.fillCircle(40, 30, 10, SSD1306_WHITE);
  display.fillCircle(60, 30, 10, SSD1306_WHITE);
  display.fillTriangle(30, 35, 70, 35, 50, 60, SSD1306_WHITE);
  
  // Draw smaller hearts around
  for (int i = 0; i < 5; i++) {
    int x = random(10, 118);
    int y = random(10, 20);
    drawSmallHeart(x, y);
  }
  
  display.display();
}

void drawSmallHeart(int x, int y) {
  display.fillCircle(x-2, y-1, 2, SSD1306_WHITE);
  display.fillCircle(x+2, y-1, 2, SSD1306_WHITE);
  display.fillTriangle(x-4, y-1, x+4, y-1, x, y+2, SSD1306_WHITE);
}

void drawEyes(int pupilX, int pupilY) {
  display.clearDisplay();
  display.drawCircle(32, 32, 20, SSD1306_WHITE); // Left eye socket
  display.drawCircle(96, 32, 20, SSD1306_WHITE); // Right eye socket
  display.fillCircle(32 + pupilX, 32 + pupilY, 8, SSD1306_WHITE); // Left pupil
  display.fillCircle(96 + pupilX, 32 + pupilY, 8, SSD1306_WHITE); // Right pupil
  
  // Draw eyelashes
  for (int i = 0; i < 3; i++) {
    display.drawLine(32-20, 32-15+i*5, 32-25, 32-18+i*5, SSD1306_WHITE); // Left eye
    display.drawLine(32+20, 32-15+i*5, 32+25, 32-18+i*5, SSD1306_WHITE); // Left eye
    display.drawLine(96-20, 32-15+i*5, 96-25, 32-18+i*5, SSD1306_WHITE); // Right eye
    display.drawLine(96+20, 32-15+i*5, 96+25, 32-18+i*5, SSD1306_WHITE); // Right eye
  }
  
  display.display();
}

void drawRomanticStory(int frame) {
  display.clearDisplay();
  
  switch(frame) {
    case 0:
      // First meeting
      drawPerson(40, 42, 1);  // Person 1
      drawPerson(88, 42, -1); // Person 2
      display.setCursor(30, 0);
      display.print("First Meeting");
      break;
      
    case 1:
      // Walking toward each other
      drawPerson(30, 42, 1);
      drawPerson(98, 42, -1);
      display.setCursor(20, 0);
      display.print("Getting Closer");
      break;
      
    case 2:
      // Close to each other
      drawPerson(50, 42, 1);
      drawPerson(78, 42, -1);
      display.setCursor(35, 0);
      display.print("Hello There!");
      break;
      
    case 3:
      // Holding hands
      drawPerson(50, 42, 1);
      drawPerson(78, 42, -1);
      display.drawLine(60, 40, 68, 40, SSD1306_WHITE); // Holding hands
      display.setCursor(30, 0);
      display.print("Holding Hands");
      break;
      
    case 4:
      // Hearts appear
      drawPerson(50, 42, 1);
      drawPerson(78, 42, -1);
      display.drawLine(60, 40, 68, 40, SSD1306_WHITE);
      drawSmallHeart(64, 20);
      display.setCursor(35, 0);
      display.print("Feeling Love");
      break;
      
    case 5:
      // More hearts
      drawPerson(50, 42, 1);
      drawPerson(78, 42, -1);
      display.drawLine(60, 40, 68, 40, SSD1306_WHITE);
      drawSmallHeart(64, 20);
      drawSmallHeart(50, 15);
      drawSmallHeart(78, 15);
      display.setCursor(20, 0);
      display.print("Falling in Love");
      break;
      
    case 6:
      // Embracing
      drawEmbracingPeople(64, 42);
      display.setCursor(25, 0);
      display.print("First Embrace");
      break;
      
    case 7:
      // Big heart
      drawEmbracingPeople(64, 42);
      drawSmallHeart(64, 15);
      drawSmallHeart(50, 10);
      drawSmallHeart(78, 10);
      display.setCursor(35, 0);
      display.print("Love Grows");
      break;
      
    case 8:
      // Proposal
      drawPerson(50, 42, 1);
      drawKneelingPerson(78, 50);
      drawSmallHeart(64, 20);
      display.setCursor(40, 0);
      display.print("Will You?");
      break;
      
    case 9:
      // Yes!
      drawPerson(50, 42, 1);
      drawPerson(78, 42, -1);
      display.drawLine(60, 40, 68, 40, SSD1306_WHITE);
      drawSmallHeart(64, 20);
      drawSmallHeart(64, 10);
      display.setCursor(50, 0);
      display.print("Yes!");
      break;
      
    case 10:
      // Wedding
      drawPerson(50, 42, 1);
      drawPerson(78, 42, -1);
      display.drawLine(60, 40, 68, 40, SSD1306_WHITE);
      // Draw wedding rings
      display.drawCircle(60, 40, 2, SSD1306_WHITE);
      display.drawCircle(68, 40, 2, SSD1306_WHITE);
      display.setCursor(25, 0);
      display.print("Together Forever");
      break;
      
    case 11:
      // Growing old together
      drawElderlyPerson(50, 42, 1);
      drawElderlyPerson(78, 42, -1);
      display.drawLine(60, 40, 68, 40, SSD1306_WHITE);
      display.setCursor(10, 0);
      display.print("Growing Old Together");
      break;
  }
  
  display.display();
}

void drawPerson(int x, int y, int direction) {
  // Head
  display.drawCircle(x, y-10, 5, SSD1306_WHITE);
  
  // Body
  display.drawLine(x, y-5, x, y+10, SSD1306_WHITE);
  
  // Arms
  display.drawLine(x, y, x-7*direction, y-5, SSD1306_WHITE);
  display.drawLine(x, y, x+7*direction, y-5, SSD1306_WHITE);
  
  // Legs
  display.drawLine(x, y+10, x-5*direction, y+20, SSD1306_WHITE);
  display.drawLine(x, y+10, x+5*direction, y+20, SSD1306_WHITE);
}

void drawElderlyPerson(int x, int y, int direction) {
  // Head with a little hunch
  display.drawCircle(x, y-8, 5, SSD1306_WHITE);
  
  // Body slightly bent
  display.drawLine(x, y-3, x, y+10, SSD1306_WHITE);
  
  // Arms with a cane if needed
  display.drawLine(x, y, x-7*direction, y-3, SSD1306_WHITE);
  display.drawLine(x, y, x+7*direction, y-3, SSD1306_WHITE);
  
  // Legs slightly bent
  display.drawLine(x, y+10, x-5*direction, y+18, SSD1306_WHITE);
  display.drawLine(x, y+10, x+5*direction, y+18, SSD1306_WHITE);
}

void drawKneelingPerson(int x, int y) {
  // Head
  display.drawCircle(x, y-15, 5, SSD1306_WHITE);
  
  // Body
  display.drawLine(x, y-10, x, y, SSD1306_WHITE);
  
  // Arms - one holding ring
  display.drawLine(x, y-5, x-10, y-10, SSD1306_WHITE);
  display.drawLine(x, y-5, x+5, y-10, SSD1306_WHITE);
  
  // Kneeling legs
  display.drawLine(x, y, x-5, y+10, SSD1306_WHITE);
  display.drawLine(x, y, x+5, y+5, SSD1306_WHITE);
  
  // Ring
  display.drawCircle(x+7, y-12, 2, SSD1306_WHITE);
}

void drawEmbracingPeople(int x, int y) {
  // First person
  display.drawCircle(x-10, y-10, 5, SSD1306_WHITE); // Head
  display.drawLine(x-10, y-5, x-10, y+10, SSD1306_WHITE); // Body
  
  // Second person
  display.drawCircle(x+10, y-10, 5, SSD1306_WHITE); // Head
  display.drawLine(x+10, y-5, x+10, y+10, SSD1306_WHITE); // Body
  
  // Arms embracing
  display.drawLine(x-10, y, x, y-5, SSD1306_WHITE);
  display.drawLine(x-10, y, x, y+5, SSD1306_WHITE);
  display.drawLine(x+10, y, x, y-5, SSD1306_WHITE);
  display.drawLine(x+10, y, x, y+5, SSD1306_WHITE);
  
  // Legs
  display.drawLine(x-10, y+10, x-15, y+20, SSD1306_WHITE);
  display.drawLine(x-10, y+10, x-5, y+20, SSD1306_WHITE);
  display.drawLine(x+10, y+10, x+5, y+20, SSD1306_WHITE);
  display.drawLine(x+10, y+10, x+15, y+20, SSD1306_WHITE);
}

void handleRoot() {
  String html = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Romantic Gift Player</title>
  <style>
    body {
      font-family: 'Arial Rounded MT Bold', 'Arial', sans-serif;
      background: linear-gradient(to bottom, #ffe6f2, #ffcce6);
      text-align: center;
      padding: 20px;
      margin: 0;
      min-height: 100vh;
    }
    h1 {
      color: #ff66b2;
      text-shadow: 2px 2px 4px rgba(0,0,0,0.1);
      margin-bottom: 10px;
    }
    .subtitle {
      color: #ff3399;
      margin-bottom: 25px;
    }
    .btn {
      background-color: #ff66b2;
      border: none;
      color: white;
      padding: 15px 25px;
      text-align: center;
      text-decoration: none;
      display: inline-block;
      font-size: 16px;
      margin: 10px 5px;
      cursor: pointer;
      border-radius: 50px;
      box-shadow: 0 4px 8px rgba(0,0,0,0.2);
      transition: 0.3s;
      width: 200px;
    }
    .btn:hover {
      background-color: #ff3399;
      box-shadow: 0 8px 16px rgba(0,0,0,0.2);
      transform: translateY(-2px);
    }
    .btn:active {
      transform: translateY(0);
    }
    .container {
      max-width: 400px;
      margin: 0 auto;
      background-color: rgba(255, 255, 255, 0.9);
      padding: 25px;
      border-radius: 20px;
      box-shadow: 0 8px 16px rgba(0,0,0,0.1);
    }
    .instructions {
      background-color: #f8f8f8;
      padding: 15px;
      border-radius: 10px;
      margin: 20px 0;
      text-align: left;
    }
    .heart {
      color: #ff66b2;
      font-size: 20px;
      margin: 0 5px;
    }
    .mode-btn {
      background-color: #ff99cc;
      margin: 5px;
      padding: 10px 15px;
      font-size: 14px;
      width: 160px;
    }
    .mode-btn.active {
      background-color: #ff66b2;
      box-shadow: 0 0 10px rgba(255, 102, 178, 0.5);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>Romantic Gift Player <span class="heart">❤</span></h1>
    <p class="subtitle">Control your romantic display while playing music from your phone</p>
    
    <div class="instructions">
      <p><b>Instructions:</b></p>
      <ol>
        <li>Play romantic music from your favorite app</li>
        <li>Connect your phone to speakers for better sound</li>
        <li>Select display mode below</li>
        <li>Enjoy the romantic atmosphere!</li>
      </ol>
    </div>
    
    <div>
      <button class="btn" onclick="playMusic()">Play Music</button>
      <button class="btn" onclick="pauseMusic()">Pause Music</button>
    </div>
    
    <h3>Display Modes:</h3>
    <div>
      <button class="btn mode-btn" id="storyBtn" onclick="setMode('story')">Love Story</button>
      <button class="btn mode-btn" id="heartsBtn" onclick="setMode('hearts')">Hearts</button>
      <button class="btn mode-btn" id="eyesBtn" onclick="setMode('eyes')">Eyes</button>
      <button class="btn mode-btn" id="messagesBtn" onclick="setMode('messages')">Messages</button>
    </div>
    
    <p style="margin-top: 20px; font-size: 14px; color: #888;">
      The display will show your love story with stick figures while you play romantic music from your phone.
    </p>
  </div>
  
  <script>
    function playMusic() {
      fetch('/play');
      alert('Play your favorite romantic music on your phone! The display will show your love story.');
    }
    
    function pauseMusic() {
      fetch('/pause');
      alert('Music control - pause your music player');
    }
    
    function setMode(mode) {
      // Update button styles
      document.querySelectorAll('.mode-btn').forEach(btn => {
        btn.classList.remove('active');
      });
      document.getElementById(mode + 'Btn').classList.add('active');
      
      // Send request to ESP32
      fetch('/' + mode);
    }
    
    // Initialize with story mode active
    window.onload = function() {
      document.getElementById('storyBtn').classList.add('active');
    }
  </script>
</body>
</html>
)=====";
  
  server.send(200, "text/html", html);
}

void handlePlay() {
  server.send(200, "text/plain", "Play music on your phone");
  displayMessage("Play romantic music on your phone!");
}

void handlePause() {
  server.send(200, "text/plain", "Pause music on your phone");
  displayMessage("Music paused");
}

void handleStory() {
  server.send(200, "text/plain", "Love story mode");
  currentState = LOVE_STORY_ANIMATION;
  currentFrame = 0;
  lastStateChange = millis();
  displayMessage("Love Story Mode");
}

void handleHearts() {
  server.send(200, "text/plain", "Hearts mode");
  currentState = HEART_ANIMATION;
  lastStateChange = millis();
  displayMessage("Hearts Mode");
}

void handleEyes() {
  server.send(200, "text/plain", "Eyes mode");
  currentState = EYES_ANIMATION;
  lastStateChange = millis();
  displayMessage("Eyes Mode");
}

void handleMessages() {
  server.send(200, "text/plain", "Messages mode");
  currentState = MESSAGE_DISPLAY;
  lastStateChange = millis();
  displayMessage("Messages Mode");
}

void handleNext() {
  server.send(200, "text/plain", "Next animation");
  currentState = static_cast<AnimationState>((currentState + 1) % 4);
  lastStateChange = millis();
}

void handlePrevious() {
  server.send(200, "text/plain", "Previous animation");
  currentState = static_cast<AnimationState>((currentState + 3) % 4);
  lastStateChange = millis();
}