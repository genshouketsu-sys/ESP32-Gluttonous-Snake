#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <DNSServer.h>
#include <WebSocketsServer.h>
#include <map>

const char* ssid = "GenShouGAME-Lab";
const char* password = "11111111";

const byte DNS_PORT = 53;
DNSServer dnsServer;
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

struct Room {
  uint8_t p1 = 255;
  uint8_t p2 = 255;
  int spectators = 0;
};

struct ClientInfo {
  String room = "";
  int role = 0; // 0=spectator, 1=P1, 2=P2
};

std::map<String, Room> rooms;
std::map<uint8_t, ClientInfo> clients;
std::vector<String> winners;

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  return "text/plain";
}

bool handleFileRead(String path) {
  if (path.endsWith("/")) path += "index.html";
  String contentType = getContentType(path);
  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void broadcastWinners() {
  String msg = "WINNERS:[";
  for(size_t i=0; i<winners.size(); i++) {
    if(i>0) msg += ",";
    msg += "\"" + winners[i] + "\"";
  }
  msg += "]";
  webSocket.broadcastTXT(msg);
}

void broadcastRoomList() {
  String list = "ROOMLIST:{";
  bool first = true;
  for (auto const& x : rooms) {
    if (!first) list += ",";
    int pCount = 0;
    if (x.second.p1 != 255) pCount++;
    if (x.second.p2 != 255) pCount++;
    list += "\"" + x.first + "\":" + String(pCount);
    first = false;
  }
  list += "}";
  webSocket.broadcastTXT(list);
}

void assignToRoom(uint8_t num, String rId) {
  ClientInfo& c = clients[num];
  Room& r = rooms[rId];
  c.room = rId;
  
  if (r.p1 == 255) {
    r.p1 = num; c.role = 1;
  } else if (r.p2 == 255) {
    r.p2 = num; c.role = 2;
  } else {
    r.spectators++; c.role = 0;
  }
  
  String msg = "ASSIGNED:" + rId + ":" + String(c.role);
  webSocket.sendTXT(num, msg);
  broadcastRoomList();
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED: {
      if (clients.count(num)) {
        String rId = clients[num].room;
        int role = clients[num].role;
        if (rId != "") {
          Room& r = rooms[rId];
          if (role == 1) r.p1 = 255;
          else if (role == 2) r.p2 = 255;
          else r.spectators--;
          
          if (r.p1 == 255 && r.p2 == 255 && r.spectators <= 0) {
            rooms.erase(rId);
          }
        }
        clients.erase(num);
        broadcastRoomList();
      }
      break;
    }
    case WStype_CONNECTED: {
      clients[num] = ClientInfo();
      broadcastRoomList();
      broadcastWinners();
      break;
    }
    case WStype_TEXT: {
      String msg = String((char*)payload);
      
      if (msg.startsWith("WINNER:")) {
        String w = msg.substring(7);
        winners.push_back(w);
        if (winners.size() > 5) winners.erase(winners.begin());
        broadcastWinners();
      }
      else if (msg == "LEAVE_ROOM") {
        if (clients.count(num)) {
          String rId = clients[num].room;
          int role = clients[num].role;
          if (rId != "") {
            Room& r = rooms[rId];
            if (role == 1) r.p1 = 255;
            else if (role == 2) r.p2 = 255;
            else r.spectators--;
            
            if (r.p1 == 255 && r.p2 == 255 && r.spectators <= 0) rooms.erase(rId);
          }
          clients[num].room = "";
          clients[num].role = 0;
          broadcastRoomList();
        }
      }
      else if (msg == "BECOME_SPECTATOR") {
        String rId = clients[num].room;
        if (rId != "") {
          int role = clients[num].role;
          if (role != 0) {
            if (role == 1) rooms[rId].p1 = 255;
            else if (role == 2) rooms[rId].p2 = 255;
            rooms[rId].spectators++;
            clients[num].role = 0;
            webSocket.sendTXT(num, "ASSIGNED:" + rId + ":0");
            broadcastRoomList();
          }
        }
      }
      else if (msg == "BECOME_PLAYER") {
        String rId = clients[num].room;
        if (rId != "") {
          int role = clients[num].role;
          if (role == 0) {
            Room& r = rooms[rId];
            if (r.p1 == 255) { r.p1 = num; r.spectators--; clients[num].role = 1; }
            else if (r.p2 == 255) { r.p2 = num; r.spectators--; clients[num].role = 2; }
            webSocket.sendTXT(num, "ASSIGNED:" + rId + ":" + String(clients[num].role));
            broadcastRoomList();
          }
        }
      }
      else if (msg.startsWith("AUTO_JOIN")) {
        // Find first room with < 2 players
        String targetRoom = "";
        for (int i=1; i<=99; i++) {
          String rId = (i < 10 ? "0" : "") + String(i);
          if (rooms.count(rId) == 0) {
            targetRoom = rId; break;
          } else {
            Room& r = rooms[rId];
            if (r.p1 == 255 || r.p2 == 255) {
              targetRoom = rId; break;
            }
          }
        }
        assignToRoom(num, targetRoom);
      } 
      else if (msg.startsWith("JOIN:")) {
        String rId = msg.substring(5);
        assignToRoom(num, rId);
      } 
      else if (msg.startsWith("MSG:")) {
        String rId = clients[num].room;
        String data = msg.substring(4);
        for (auto const& c : clients) {
          if (c.second.room == rId && c.first != num) {
            webSocket.sendTXT(c.first, data);
          }
        }
      }
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed");
    return;
  }

  WiFi.softAP(ssid, password, 1, 0, 8); // Channel 1, Hidden 0, Max connections 8
  IPAddress IP = WiFi.softAPIP();
  dnsServer.start(DNS_PORT, "*", IP);

  server.onNotFound([]() {
    if (handleFileRead(server.uri())) return;
    server.sendHeader("Location", String("http://") + server.client().localIP().toString(), true);
    server.send(302, "text/plain", "");
  });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  webSocket.loop();
}
