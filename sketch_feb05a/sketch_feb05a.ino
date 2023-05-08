#include <SPI.h>
#include <WiFiNINA.h>

// Replace with your network credentials
const char* ssid = "Zelda";
const char* password = "butterflynet11";

// Web server address and port
WiFiServer server(80);

void setup() {
  // initialize digital pins as outputs
  for (int i = 0; i < 13; i++) {
    pinMode(i, OUTPUT);
  }

  // initialize serial communication
  Serial.begin(9600);

  // connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // start the web server
  server.begin();
  Serial.println("Web server started");
  Serial.println(WiFi.localIP());
}

void loop() {
  // listen for incoming clients
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // read the request and parse the command
  String request = client.readStringUntil('\r');
  int firstSpace = request.indexOf(' ');
  int secondSpace = request.indexOf(' ', firstSpace + 1);
  String command = request.substring(firstSpace + 1, secondSpace);

  // toggle digital outputs or read analog inputs based on the command
  if (command.startsWith("/digital")) {
    int pin = command.substring(8).toInt();
    int state = digitalRead(pin);
    digitalWrite(pin, !state);
  } else if (command.startsWith("/analog")) {
    int pin = command.substring(7).toInt();
    int value = analogRead(pin);
  }

  // send the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<html>");
  client.println("<head><title>Arduino RP2040 Web Server</title></head>");
  client.println("<body>");
  client.println("<h1>Arduino RP2040 Web Server</h1>");
  client.println("<p>All digital outputs:</p>");
  client.println("<ul>");
  for (int i = 0; i < 13; i++) {
    int state = digitalRead(i);
    client.print("<li>Digital Pin ");
    client.print(i);
    client.print(": <a href='/digital");
    client.print(i);
    client.print("'>");
    client.print(state ? "ON" : "OFF");
    client.println("</a></li>");
  }
  client.println("</ul>");
  client.println("<p>All analog inputs:</p>");
  client.println("<ul>");
  for (int i = 0; i < 6; i++) {
    int value = analogRead(i);
    client.print("<li>Analog Pin A");
    client.print(i);
    client.print(": ");
    client.print(value);
    client.println("</li>");
  }
  client.println("</ul>");
  client.println("</body>");
  client.println("</html>");
  client.stop();
}
