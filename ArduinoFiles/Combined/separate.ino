#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <Wire.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define SDA 2
#define SCL 2
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
int red = 4;
int green = 3;

LiquidCrystal_I2C lcd(0x3F, 16, 2);
String bootstrap = " .form-control { display: block; width: 100%; height: 34px; padding: 6px 12px; font-size: 14px; line-height: 1.42857143; color: #555; background-color: #fff; background-image: none; border: 1px solid #ccc; border-radius: 4px; -webkit-box-shadow: inset 0 1px 1px rgba(0, 0, 0, .075); box-shadow: inset 0 1px 1px rgba(0, 0, 0, .075); -webkit-transition: border-color ease-in-out .15s, -webkit-box-shadow ease-in-out .15s; -o-transition: border-color ease-in-out .15s, box-shadow ease-in-out .15s; transition: border-color ease-in-out .15s, box-shadow ease-in-out .15s; } .form-control:focus { border-color: #66afe9; outline: 0; -webkit-box-shadow: inset 0 1px 1px rgba(0,0,0,.075), 0 0 8px rgba(102, 175, 233, .6); box-shadow: inset 0 1px 1px rgba(0,0,0,.075), 0 0 8px rgba(102, 175, 233, .6); } .form-group { margin-bottom: 15px; } .help-block { display: block; margin-top: 5px; margin-bottom: 10px; color: #737373; } .btn { display: inline-block; padding: 6px 12px; margin-bottom: 0; font-size: 14px; font-weight: normal; line-height: 1.42857143; text-align: center; white-space: nowrap; vertical-align: middle; -ms-touch-action: manipulation; touch-action: manipulation; cursor: pointer; -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-image: none; border: 1px solid transparent; border-radius: 4px; } .btn:focus, .btn:active:focus, .btn.active:focus, .btn.focus, .btn:active.focus, .btn.active.focus { outline: 5px auto -webkit-focus-ring-color; outline-offset: -2px; } .btn:hover, .btn:focus, .btn.focus { color: #333; text-decoration: none; } .btn:active, .btn.active { background-image: none; outline: 0; -webkit-box-shadow: inset 0 3px 5px rgba(0, 0, 0, .125); box-shadow: inset 0 3px 5px rgba(0, 0, 0, .125); } .btn-primary { color: #fff; background-color: #337ab7; border-color: #2e6da4; } .btn-primary:focus, .btn-primary.focus { color: #fff; background-color: #286090; border-color: #122b40; } .btn-primary:hover { color: #fff; background-color: #286090; border-color: #204d74; } .btn-primary:active, .btn-primary.active, .open > .dropdown-toggle.btn-primary { color: #fff; background-color: #286090; border-color: #204d74; } .btn-primary:active:hover, .btn-primary.active:hover, .open > .dropdown-toggle.btn-primary:hover, .btn-primary:active:focus, .btn-primary.active:focus, .open > .dropdown-toggle.btn-primary:focus, .btn-primary:active.focus, .btn-primary.active.focus, .open > .dropdown-toggle.btn-primary.focus { color: #fff; background-color: #204d74; border-color: #122b40; } .btn-primary:active, .btn-primary.active, .open > .dropdown-toggle.btn-primary { background-image: none; } table {border-spacing: 0;border-collapse: collapse;} .table {width: 100%;max-width: 100%;margin-bottom: 20px;}.table > thead > tr > th,.table > tbody > tr > th,.table > tfoot > tr > th,.table > thead > tr > td,.table > tbody > tr > td,.table > tfoot > tr > td {padding: 8px;line-height: 1.42857143;vertical-align: top;border-top: 1px solid #ddd;}.table > thead > tr > th {vertical-align: bottom;border-bottom: 2px solid #ddd;}.table > caption + thead > tr:first-child > th,.table > colgroup + thead > tr:first-child > th,.table > thead:first-child > tr:first-child > th,.table > caption + thead > tr:first-child > td,.table > colgroup + thead > tr:first-child > td,.table > thead:first-child > tr:first-child > td {border-top: 0;}.table > tbody + tbody {border-top: 2px solid #ddd;}.table .table {background-color: #fff;} .table-bordered {border: 1px solid #ddd;}.table-bordered > thead > tr > th,.table-bordered > tbody > tr > th,.table-bordered > tfoot > tr > th,.table-bordered > thead > tr > td,.table-bordered > tbody > tr > td,.table-bordered > tfoot > tr > td { border: 1px solid #ddd;}.table-bordered > thead > tr > th,.table-bordered > thead > tr > td {border-bottom-width: 2px;} .table-hover > tbody > tr:hover {background-color: #f5f5f5;}";
String CurrClub;
String line1 = "BY ROBOTICS & CIRCUITS ";
String LastFlash="";
int stringStart, stringStop = 0;
int screenWidth = 16;
int screenHeight = 2;
int scrollCursor = screenWidth;
const char* ssid = "login";                //hotspot id
const char* password = "passwords";        //hotspot pass

struct MasterAdmin {
    String username;
    String password;
    String club;
};
MasterAdmin MA[5] = {
  {"root","toor","none"},
  {"Maintenence","formaintenaceonly","Workshop"},
  {"Head","tail","PickOne"}
};

struct Admin {
    String username;
    String password;
    String club;
  public:
    Admin() {
      username = "";
      password = "";
      club = "";
    }
} admins[40];

struct Member {
  String uid;
  String uname;
  String club;
  Member() {
    uid = "";
    uname = "";
    club = "";
  }
  void insert(String id, String u, String c) {
    uid = id;
    uname = u;
    club = c;
  }
} members[100];

struct Activity {
  String uid;
  String uname;
  String club;
  Activity() {
    uid = "";
    uname = "";
    club = "";
  }
  void insert(String id, String u, String c) {
    uid = id;
    uname = u;
    club = c;
  }
} activities[100];

ESP8266WebServer server(80);

//Check if header is present and correct
bool is_authentified() {
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

bool is_Admin_authentified() {
  Serial.println("Enter is_authentified");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ADMINCONTROL=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  return false;
}

//login page for admins, also called for disconnect
void handleAdmin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnected");
    server.sendHeader("Location", "/admins");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.sendHeader("Set-Cookie", "ADMINCONTROL=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    for (int i = 0; i < sizeof(MA) / sizeof(MA[0]); i++)
    {
      if (server.arg("USERNAME") == MA[i].username &&  server.arg("PASSWORD") == MA[i].password) {
        server.sendHeader("Location", "/landadmin");
        server.sendHeader("Cache-Control", "no-cache");
        server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
        server.sendHeader("Set-Cookie", "ADMINCONTROL=1");
        CurrClub = MA[i].club;
        server.send(301);
        Serial.println("Log in Successful");
        return;
      }
    }
    msg = "Wrong username/password! try again. Or try general admin login <a href=\"/handleLogin\">here</a>";
    Serial.println("Log in Failed");
  }

  String content = "<html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Admin Login</title><style type='text/css'> body{ font: 14px sans-serif; } .wrapper{ width: 350px; padding: 20px; left: 50%; top: 50%; position: fixed; transform: translate(-50%, -50%); }" + bootstrap + "</style></head><body><div class='wrapper'><h2>Admin Login</h2><p>Please fill in your Master Admin credentials to login.</p><form action='/admins' method='post'><div class='form-group'><label>Username</label><input type='text' name='USERNAME' class='form-control' placeholder='username'></div><div class='form-group'><label>Password</label><input type='password' name='PASSWORD' class='form-control' placeholder='password'><span class='help-block'>" + msg + "</span></div><div class='form-group'><input type='submit' class='btn btn-primary' value='Submit'></div></form></div></br></br></br></br><div class=\"links\">If you are General Admin, please visit <a href=\"/login\">here</a></div></body>";
  server.send(200, "text/html", content);
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnected");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.sendHeader("Set-Cookie", "ADMINCONTROL=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    for (int i = 0; i < sizeof(admins) / sizeof(admins[0]); i++)
    {
      if (server.arg("USERNAME") == admins[i].username &&  server.arg("PASSWORD") == admins[i].password) {
        server.sendHeader("Location", "/");
        server.sendHeader("Cache-Control", "no-cache");
        server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
        server.sendHeader("Set-Cookie", "ADMINCONTROL=0");
        CurrClub = admins[i].club;
        server.send(301);
        Serial.println("Log in Successful");
        return;
      }
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }

  String content = "<html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1'><title>Login</title><style type='text/css'> body{ font: 14px sans-serif; } .wrapper{ width: 350px; padding: 20px; left: 50%; top: 50%; position: fixed; transform: translate(-50%, -50%); }" + bootstrap + "</style></head><body><div class='wrapper'><h2>Login</h2><p>Please fill in your credentials to login.</p><form action='/login' method='post'><div class='form-group'><label>Username</label><input type='text' name='USERNAME' class='form-control' placeholder='username'></div><div class='form-group'><label>Password</label><input type='password' name='PASSWORD' class='form-control' placeholder='password'><span class='help-block'>" + msg + "</span></div><div class='form-group'><input type='submit' class='btn btn-primary' value='Submit'></div></form></div></br></br></br></br><div class=\"links\">If you are the <strong>Master Admin</strong>, To manage <strong>Admins</strong>visit <a href=\"/admins\">here</a></div></body>";
  server.send(200, "text/html", content);
}

//root page can be accessed only if authentification is ok
void handleAdminRoot() {
  String msg = "";
  Serial.println("Enter handleAdminRoot");
  String header;
  if (!is_Admin_authentified()) {
    server.sendHeader("Location", "/admins");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  if (server.hasArg("NOUID")) {
    msg += "No Username was supplied!";
  }
  String content = "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>Admin Dashboard</title> <style>" + bootstrap + "</style> </head> <body> <div class=\"form\"> <p>Welcome to Admin Dashboard.</p> <p><a href=\"/landadmin\">Home</a><p> <p><a href=\"/insert\">Insert New Record</a></p> <p><a href=\"/view\">View Records</a><p> <p><a href=\"/edit\">Edit Records</a><p> <p><a href=\"/login?DISCONNECT=YES\">Logout</a></p> <br /><br /><br /><br /><span class='help-block'>" + msg + "</span> </div> </body> </html>";
  server.send(200, "text/html", content);
}


//root page can be accessed only if authentification is ok
void handleRoot() {
  String msg = "";
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authentified()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  if (server.hasArg("AUTHORIZE")) {
    msg += "You're not authorized to perform the action!";
  }
  if (server.hasArg("NOUID")) {
    msg += "No UID was supplied!";
  }
  String content = "<!DOCTYPE html> <html lang=\"en\"> <head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>Dashboard - RNC</title> <style>" + bootstrap + "</style> </head> <body> <div class=\"form\"> <p>Welcome to Dashboard.</p> <p><a href=\\>Home</a><p> <p><a href=\"/insert\">Insert New Record</a></p> <p><a href=\"/view\">View Records</a><p> <p><a href=\"/edit\">Edit Records</a><p> <p><a href=\"/login?DISCONNECT=YES\">Logout</a></p> <br /><br /><br /><br /><span class='help-block'>" + msg + "</span> </div> </body> </html>";
  server.send(200, "text/html", content);
}

void handleDelete() {
  String msg;
  String cookie = server.header("Cookie");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.sendHeader("Set-Cookie", "ADMINCONTROL=0");
    server.send(301);
    return;
  }
  if (server.hasArg("UID")) {
    if (cookie.indexOf("ADMINCONTROL=0") != -1) {
      uint memaddr = 0;
      for (int i = 0; i < 20; i++)
      {
        if (server.arg("UID") == members[i].uid) {
          if (members[i].club == CurrClub) {
            EEPROM.get(memaddr, members);
            members[i].uid = "";
            members[i].uname = "";
            members[i].club = "";
            uint addr = 0;
            EEPROM.put(memaddr, members);
            EEPROM.commit();
            Serial.println("Deleted Successful");
            server.sendHeader("Location", "/view");
            server.sendHeader("Cache-Control", "no-cache");
            server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
            server.sendHeader("Set-Cookie", "ADMINCONTROL=0");
            server.send(301);
            return;
          }
          Serial.print("Not Authorized");
          server.sendHeader("Cache-Control", "no-cache");
          server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
          server.sendHeader("Location", "/?NOTAUTHORIZE=0");
          server.send(301);
          return;
        }
      }
    }
    if (cookie.indexOf("ADMINCONTROL=1") != -1) {
      uint memaddr = 0;
      for (int i = 0; i < 20; i++)
      {
        if (server.arg("UID") == admins[i].username) {
          EEPROM.get(memaddr, admins);
          admins[i].username = "";
          admins[i].password = "";
          admins[i].club = "";
          uint addr = 0;
          EEPROM.put(memaddr, admins);
          EEPROM.commit();
          Serial.println("Deleted Successful");
          server.sendHeader("Location", "/view");
          server.sendHeader("Cache-Control", "no-cache");
          server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
          server.sendHeader("Set-Cookie", "ADMINCONTROL=1");
          server.send(301);
          return;
        }
      }
    }
  }
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
  String tmp;
  if(cookie.indexOf("ADMINCONTROL=1") != -1) tmp="landadmin";
  else tmp="";
  server.sendHeader("Location", "/"+tmp+"?NOUID=0");
  Serial.println("No UID supplied");
  server.send(301);
}


void handleEdit() {
  String msg = "";
  String cookie = server.header("Cookie");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.sendHeader("Set-Cookie", "ADMINCONTROL=0");
    server.send(301);
    return;
  }
  if (server.hasArg("UID") && server.hasArg("UNAME"))
  {
    if (cookie.indexOf("ADMINCONTROL=0") != -1) {
      uint memaddr = 0;
      int count;
      EEPROM.get(memaddr, members);
      for (count = 0; members[count].uid != ""; count++) {
        if (members[count].uid == server.arg("UID")) {
          if (members[count].club == CurrClub) {
            Serial.print(server.arg("UID"));
            members[count].uname = server.arg("UNAME");
            msg += "Successfully Updated. <a href=\"/view\">View Records</a>";
            EEPROM.put(memaddr, members);
            EEPROM.commit();
            break;
          }
          Serial.print("Not Authorized!");
          msg+= "Not authorized to perform the action";
          break;
        }
        Serial.print("UID Not Found!");
        msg+= "UID not Found";
        break;
      }
    }
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD") && server.hasArg("CLUB"))
  {
    if (cookie.indexOf("ADMINCONTROL=1") != -1) {
      uint memaddr = 0;
      int count;
      EEPROM.get(memaddr, admins);
      for (count = 0; admins[count].username != ""; count++) {
        if (admins[count].username == server.arg("USERNAME")) {
          Serial.print(server.arg("USERNAME"));
          admins[count].password = server.arg("PASSWORD");
          admins[count].club = server.arg("CLUB");
          msg += "Successfully Updated. <a href=\"/view\">View Records</a>";
          EEPROM.put(memaddr, admins);
          EEPROM.commit();
          break;
        }
        Serial.print("Username Not Found!");
        msg+= "Username not found";
        break;
      }
    }
  }
  String content = "<html> <head> <meta charset=\"utf-8\"> <title>Update Record</title> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <style type=\"text/css\"> body{ font: 14px sans-serif; } .wrapper{ width: 350px; padding: 20px; left: 50%; top: 50%; position: fixed; transform: translate(-50%, -50%); } " + bootstrap + " </style> </head> <body> <div class=\"wrapper\"> <div> <h1>Update Record</h1> <form action=\"/edit\" method=\"post\"> <div class=\"form-group\">";
  if (cookie.indexOf("ADMINCONTROL=0") != -1) content += "<p><input type=\"text\" name=\"UID\" placeholder=\"UserID\" class=\"form-control\" required /></p> </div> <div class=\"form-group\"> <p><input type=\"text\" name=\"UNAME\" placeholder=\"UserName\" class=\"form-control\" required /></p> </div>";
  if (cookie.indexOf("ADMINCONTROL=1") != -1) content += "<p><input type=\"text\" name=\"USERNAME\" placeholder=\"Username\" class=\"form-control\" required /></p> </div> <div class=\"form-group\"> <p><input type=\"text\" name=\"PASSWORD\" placeholder=\"Password\" class=\"form-control\" required /></p> </div> <div class=\"form-group\"> <p><input type=\"text\" name=\"CLUB\" placeholder=\"Password\" class=\"form-control\" required /></p> </div>";
  content += "<div class=\"form-group\"> <p><input name=\"submit\" type=\"submit\" class=\"btn btn-primary\" value=\"Update\" /></p> </div> </form> <p style=\"color:#FF0000;\">" + msg + "</p> <br /><br /><br /><br /> </div <div class=\"links\"> <p><a href=\"/\">Dashboard</a> | <a href=\"/insert\">Insert New Record</a> | <a href=\"login?DISCONNECT=YES\">Logout</a></p> </div> </div> </body> </html>";
  server.send(200, "text/html", content);
}



void handleInsert() {
  String msg = "";
  String cookie = server.header("Cookie");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("UID") && server.hasArg("UNAME"))
  {
    if (cookie.indexOf("ADMINCONTROL=0") != -1) {
      int count;
      uint memaddr = 0;
      EEPROM.get(memaddr, members);
      for (count = 0; members[count].uid != ""; count++);
      members[count].uid = server.arg("UID");
      members[count].uname = server.arg("UNAME");
      members[count].club = CurrClub;
      msg += "Successfully Inserted. <a href=\"/view\">View Records</a>";
      EEPROM.put(memaddr, members);
      EEPROM.commit();
    }
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD") && server.hasArg("CLUB"))
  {
    if (cookie.indexOf("ADMINCONTROL=1") != -1) {
      int count;
      uint memaddr = 0;
      EEPROM.get(memaddr, admins);
      for (count = 0; admins[count].username != ""; count++);
      admins[count].username = server.arg("USERNAME");
      admins[count].password = server.arg("PASSWORD");
      admins[count].club = server.arg("CLUB");
      msg += "Successfully Inserted. <a href=\"/view\">View Records</a>";
      EEPROM.put(memaddr, admins);
      EEPROM.commit();
    }
  }
  String content = "<html> <head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>Insert New Record</title> <style type=\"text/css\"> body{ font: 14px sans-serif; } .wrapper{ width: 350px; padding: 20px; left: 50%; top: 50%; position: fixed; transform: translate(-50%, -50%); }" + bootstrap + "</style> </head> <body> <div class=\"wrapper\"> <div> <h1>Insert New Record</h1> <form action=\"/insert\" method=\"post\"> <div class=\"form-group\">";
  if (cookie.indexOf("ADMINCONTROL=0") != -1) content += "<p><input type=\"text\" name=\"UID\" placeholder=\"UserID\" class=\"form-control\" required /></p> </div> <div class=\"form-group\"> <p><input type=\"text\" name=\"UNAME\" placeholder=\"UserName\" class=\"form-control\" required /></p>";
  if (cookie.indexOf("ADMINCONTROL=1") != -1) content += "<p><input type=\"text\" name=\"USERNAME\" placeholder=\"Username\" class=\"form-control\" required /></p> </div> <div class=\"form-group\"> <p><input type=\"text\" name=\"PASSWORD\" placeholder=\"Password\" class=\"form-control\" required /></p></div><div class=\"form-group\"> <p><input type=\"text\" name=\"PASSWORD\" placeholder=\"Password\" class=\"form-control\" required /></p>";
  content += "<span class=\"help-block\">" + msg + "</span> </div><div class=\"form-group\"> <p><input name=\"submit\" type=\"submit\" value=\"Submit\" class=\"btn btn-primary\"/></p> </div> </form> <br /><br /><br /><br /> </div> <div class=\"links\"> <p><a href=\"/\">Dashboard</a> | <a href=\"/view\">View Records</a> | <a href=\"/login?DISCONNECT=YES\">Logout</a></p> </div> </div> </body> </html>";
  server.send(200, "text/html", content);
}



void handleView() {
  String msg;
  String cookie = server.header("Cookie");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("AUTHORIZE")) {
    msg += "You're not authorized to perform the action!";
  }
  if (server.hasArg("NOUID")) {
    msg += "No UID was supplied!";
  }
  String content = "<html> <head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"> <title>View Records</title> </head> <style>" + bootstrap + "</style><body> <div class=\"form\"> <p><a href=\"/\">Home</a> | <a href=\"/insert\">Insert New Record</a> | <a href=\"/login?DISCONNECT=YES\">Logout</a></p> <h2>View Records</h2> <table class=\"table table-bordered table-hover\"> <thead> <tr> <th><strong>S.No</strong></th> <th><strong>UserID</strong></th> <th><strong>UserName</strong></th> <th><strong>Edit</strong></th><th> <strong>Delete</strong></th> </tr> </thead> <tbody>";
  if (cookie.indexOf("ADMINCONTROL=0") != -1) {
    int count = 1;
    for (int i = 0; i < 20; i++)
    {
      if (members[i].uid != "") {
        content += "<tr> <td align=\"center\">" + String(count) + "</td> <td align=\"center\">" + String(members[i].uid) + "</td> <td align=\"center\">" + String(members[i].uname) + "</td> <td align=\"center\"><a href=\"/edit?UID=" + String(members[i].uid) + "\">Edit</a></td> <td align=\"center\"><a href=\"delete?UID=" + String(members[i].uid) + "\">Delete</a></td> </tr>";
        count++;
      }
    }
    content += "</tbody> </table> <br /><br /><br /><br /> </div> </body>";
    server.send(200, "text/html", content);
  }
  if (cookie.indexOf("ADMINCONTROL=1") != -1) {
    int count = 1;
    for (int i = 0; i < 20; i++)
    {
      if (admins[i].username != "") {
        content += "<tr> <td align=\"center\">" + String(count) + "</td> <td align=\"center\">" + String(admins[i].username) + "</td> <td align=\"center\">" + String(admins[i].password) + "</td> <td align=\"center\"><a href=\"/edit?UID=" + String(admins[i].username) + "\">Edit</a></td> <td align=\"center\"><a href=\"delete?UID=" + String(admins[i].username) + "\">Delete</a></td> </tr>";
        count++;
      }
    }
    content += "</tbody> </table> <br /><br /><br /><br /> </div> </body>";
    server.send(200, "text/html", content);     
  }
}

//no need authentification
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  Serial.begin(115200);
  EEPROM.begin(512);

  Wire.begin(SDA, SCL);
  lcd.init();
  lcd.backlight(); // Enable or Turn On the backlight
  lcd.setCursor(0, 0);
  lcd.print("INITIALIZING...");
  delay(500);
 
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  
  lcd.clear();
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());


  server.on("/", handleRoot);
  server.on("/login", handleLogin);
  server.on("/insert", handleInsert);
  server.on("/delete", handleDelete);
  server.on("/edit", handleEdit);
  server.on("/view", handleView);
  server.on("/admins", handleAdmin);
  server.on("/landadmin", handleAdminRoot);
  //server.on("/inline", []() {
  //server.send(200, "text/plain", "this works without need of authentification");
  //});

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  Serial.println();
  Serial.println("\nApproximate your card to the reader...");
  lcd.setCursor(0,0);
  lcd.print("Flash Your Card");
  lcd.setCursor(1,1);
  scroll();
  lcd.print(line1.substring(stringStart,stringStop));

  //Look for card approximation
  while ( ! mfrc522.PICC_IsNewCardPresent())
  {
    server.handleClient();
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
  {
    return;
  }
    //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i],HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if(LastFlash!=content){
    for(int i = 0; i < sizeof(members) / sizeof(members[0]); i++)
    {
      if(members[i].uid==content)
      {
        if(LastFlash=="") LastFlash=content;
        for(int j=0;activities[j].uid!="";j++){
          activities[i].uid=content;
          activities[i].uname=members[i].uname;
          activities[i].club=members[i].club;
        }
      }
    }
  }
}
int str_length()
{
  if(line1)
  {
    return line1.length();
  }
}
void scroll()
{
  lcd.setCursor(scrollCursor, 1);
   if(stringStart == 0 && scrollCursor > 0){
    scrollCursor--;
    stringStop++;
  } else if (stringStart == stringStop){
    stringStart = stringStop = 0;
    scrollCursor = screenWidth;
  } else if (stringStop == str_length() && scrollCursor == 0) {
    stringStart++;
  } else {
    stringStart++;
    stringStop++;
  }
  delay(300);
}
