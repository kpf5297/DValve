#include <WiFiNINA.h>
#include <WiFiClient.h>
#include <WebServer.h>


// Replace with your network credentials
const char *ssid = "IngMarWIFI_1872";
const char *password = "ingmarwifi";

// Create a web server object
WebServer server(80);

// Pin where the digital output is connected
int digitalOutputPin = 12;

// Handle requests to the root URL
void handleRoot() {
  // Generate the HTML content
  String html = "<html><body>";
  html += "<h1>Digital Output Control</h1>";
  html += "<a href='/on'>Turn On</a><br>";
  html += "<a href='/off'>Turn Off</a>";
  html += "</body></html>";

  // Send the HTML content to the client
  server.send(200, "text/html", html);
}

// Handle requests to turn the digital output on
void handleOn() {
  // Set the digital output to high
  digitalWrite(digitalOutputPin, HIGH);

  // Call the root URL handler
  handleRoot();
}

// Handle requests to turn the digital output off
void handleOff() {
  // Set the digital output to low
  digitalWrite(digitalOutputPin, LOW);

  // Call the root URL handler
  handleRoot();
}

void setup() {
  // Connect to the WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }

  // Set the digital output pin as an output
  pinMode(digitalOutputPin, OUTPUT);

  // Attach the URL handlers to the server
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);

  // Start the web server
  server.begin();
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}
