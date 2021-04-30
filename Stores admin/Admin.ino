#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "FPS_GT511C3.h"
#include <LittleFS.h>

FPS_GT511C3 fps(4, 5); // (Arduino SS_RX = pin 4, Arduino SS_TX = pin 5)
const char MAIN_page[] =R"=====(
<!--AumSriSaiRam-->
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Biometric Authentication</title>
    <style>
        input[type="text"]
        {
            width: 10em;
            height: 2em;
            border-radius: 0px;
            background-color: drakcyan;

        }
        body{
            background-color: #cbf6fc;
        }

        .box {
            background-color: #0cb4c9;
            width: 66em;
            height: 11em;
            border-radius: 10px;
            text-align: center;
            font-family: 'Trebuchet MS', 'Lucida Sans Unicode', 'Lucida Grande', 'Lucida Sans', Arial, sans-serif;
            
        }
        .box:hover{
            background: linear-gradient(90deg,#0cb4c9,rgb(17, 248, 248),#0cb4c9);
        }

        #box1 {
            position: absolute;
            left: 10%;
            top: 15%;
           
        }

        #box2 {
            position: absolute;
            left: 10%;
            top: 50%;
        }

        #box3 {
            position: absolute;
            left: 10%;
            top: 85%;
        }

        #box4 {
            position: absolute;
            left: 10%;
            top: 120%;
        }

        #box5 {
            position: absolute;
            left: 10%;
            top: 155%;
        }

        #btn {
            color: #fff;
            background-color: #dc3545;
            border-color: #dc3545;
            height: 3em;
            width: 10em;
            position: absolute;
            left: 65%;
            border-radius: 8px;

        }
        #btn:hover{
            background-color: #e64252;
            border-color: #e64252;
        }

        #title{
            background: linear-gradient(90deg,#0cb4c9,rgb(17, 248, 248),#0cb4c9);
            width: 30em;
            height: 3em;
            border-radius: 10px;
            position: absolute;
            left: 32%;
            
        }

        #text{
            position: absolute;
            top: -80%;
            left: 28%;
            text-align: center;
            font-size: 2em;
            font-family: Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;
            color: #fff;
        }
    </style>
    <script>
        function validate(form)
        {
            if(document.Menu.reg.value < 0 ||document.Menu.reg.value > 999999)
            {
                    alert("Enter valid Registration number");
                    document.Menu.reg.value="";
                    document.Menu.reg.focus();
                    return false;
            }
            if(isNaN(document.Menu.reg.value))
            {
                alert("Registration number must be a number");
                document.Menu.reg.value="";
                document.Menu.reg.focus();
                return false;
            }
            return true;
        }
        function validate1(form)
        {
            if(document.Menu1.reg1.value < 0 ||document.Menu1.reg1.value > 999999)
            {
                    alert("Enter valid Registration number");
                    document.Menu1.reg1.value="";
                    document.Menu1.reg1.focus();
                    return false;
            }
            if(isNaN(document.Menu1.reg1.value))
            {
                alert("Registration number must be a number");
                document.Menu1.reg1.value="";
                document.Menu1.reg1.focus();
                return false;
            }
            return true;
        }
    </script>
</head>

<body>
    <div id="title">
        <div id="text">
        <h4>Admin Menu</h4>
        </div>
    </div>
    <div id="box1" class="box">
        <form name="Menu" onSubmit="return validate(this)" action="/enroll" method="post">
            <p>1.Enter Registration number.</p>
            <p>2.Click on ENROLL button.</p>
            <p>3.Press your finger thrice on sensor.</p>
            <input type="text" id="reg" name="reg" placeholder="Registration number" required>
            <button name="btn" value="0" id="btn">ENROLL</button>
        </form>

    </div>
    <br>
    <div id="box2" class="box">
        <form action="/verify" method="post">
            <p>1.Click on VERIFY button.</p>
            <p>2.Press Finger on sensor.</p><br>
            <button name="btn" value="2" id="btn">VERIFY</button>
        </form>
    </div>
    <br>
    <div id="box3" class="box">
        <form name="Menu1" onSubmit="return validate1(this)" action="/delete" method="post">
            <p>1.Enter Registration number.</p>
            <p>2.Click on DELETE button.</p>
            <input type="text" id="reg1" name="reg1" placeholder="Registration number" required>
            <button name="btn" value="1" id="btn">DELETE</button>
        </form>
    </div>
    <br>
    <div id="box4" class="box">
        <form action="/Admin_change" method="post">
            <p>1.Click on ADMIN_CHANGE button.</p>
            <p>2.Press your finger thrice on sensor.</p><br>
            <button name="btn" value="3" id="btn">ADMIN_CHANGE</button>
        </form>
    </div>
    <div id="box5" class="box">
        <form action="/ShowId" method="post">
            <p>1.Click on SHOW_ID'S button.</p>
            <p>2.It will show all enrolled id's.</p><br>
            <button name="btn" value="4" id="btn">SHOW_ID'S</button>
        </form>
    </div>
</body>

</html> 
)=====";

const char RESULT_page[] = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Result</title>
    <style>
        #form {
            background-color: #0cb4c9;
            width: 550px;
            position: absolute;
            left: 33%;
            top: 19%;
            height: 12em;
            border-radius: 20px;
            text-align: center;
            
        }
        #form:hover{
            background: linear-gradient(90deg,#0cb4c9,rgb(17, 248, 248),#0cb4c9);
        }

         body{
            background-color: #cbf6fc;
        }

        #btn {
            color: #fff;
            background-color: #dc3545;
            border-color: #dc3545;
            height: 3em;
            width: 10em;
            left: 0%;
            top: 250%;
            border-radius: 8px;
        }

        #btn:hover {
            background-color: #f05262;
            border-color: #f05262;
        }
        p
        {
          padding: 30px;
          font-family: Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;
        }
    </style>
</head>

<body>
  <form action="/" method="post">
    <div id="form">
            <p>@@@</p>
            <button id="btn">MENU</button>
    </div>
   </form>
</body>

</html>

)=====";
 
//SSID and Password of your WiFi router
const char* ssid = "NETGEAR24";
const char* password = "omsrisairam";
 
ESP8266WebServer server(80); //Server on port 80
 

void setup(void)
{
  Serial.begin(115200);
  fps.Open();
  fps.SetLED(true);
  WiFi.begin(ssid, password);  
  Serial.println("");
  if (!LittleFS.begin())
  {
    Serial.println("LittleFS mount failed");
    return;
  } 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);  
  server.on("/enroll", handleEnroll);
  server.on("/verify", handleVerify);
  server.on("/delete", handleDelete);
  server.on("/Admin_change", handleAdmin_change);
  server.on("/ShowId", handleShowId);
  server.begin(); 
  Serial.println("HTTP server started");
}

void handleRoot() 
{
  server.send(200, "text/html", MAIN_page); //Send web page
}

void handleShowId()
{
  String s=RESULT_page;
  String data = getId();
  if(data == "")
    s.replace("@@@","No Users");
  else
    s.replace("@@@",data);
  server.send(200, "text/html", s);
}

void handleEnroll()
{
  String reg =server.arg("reg");
  String s=RESULT_page;
  int i = Enroll(reg,1);
  if(i == -1)
     s.replace("@@@","Failed to enroll");
  else if(i == -4)
     s.replace("@@@","Registration number already Exists ");
  else if(i == -2)
     s.replace("@@@","Bad Finger use another Finfer");
  else if(i == -3)
     s.replace("@@@","Finger print already Exists");
  else if(i == -10)
     s.replace("@@@","Time expired");
  else
     s.replace("@@@","Enrolling successfull with Registration number: "+reg);
  server.send(200, "text/html", s); 
}

void handleDelete()
{
  String reg =server.arg("reg1");
  String s=RESULT_page;
  int i = Deleteid(reg);
  if(i == -1)
     s.replace("@@@","No match found with Registration number: "+reg);
  else
     s.replace("@@@","Finger print with Registration number: "+reg+" is deleted");
  server.send(200, "text/html", s); 
}

void handleVerify()
{
  uint32_t i =Verify();
  String reg=String(i);
  String s=RESULT_page;
  if(i == -10)
    s.replace("@@@","Time expired");
  else if(i != -1)
    s.replace("@@@","Finger print found with Registration number: "+reg);   
  else
    s.replace("@@@","No match found");
  server.send(200, "text/html", s);
}

void handleAdmin_change()
{
  uint32_t i =adminChange();
  String s=RESULT_page;
  if(i == -2)
     s.replace("@@@","Bad Finger use another Finfer");
  if(i == -10)
     s.replace("@@@","Time Expired");
  else if(i == -3)
     s.replace("@@@","Finger Print already Exists");
  else if(i != -1)
    s.replace("@@@","Welcome new admin");
  else
    s.replace("@@@","Enroll failed");
  server.send(200, "text/html", s);
}

void loop()
{
  server.handleClient();
}
