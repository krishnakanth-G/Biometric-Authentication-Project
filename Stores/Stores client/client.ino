#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "FPS_GT511C3.h"
#include <LittleFS.h>

FPS_GT511C3 fps(4, 5); // (Arduino SS_RX = pin 4, Arduino SS_TX = pin 5)

const char STORES_page[] = R"=====(
<!--AumSriSaiRam-->
<!DOCTYPE html>
<html lang="en">
 <head>
    <meta charset="UTF-8">
    <title>STORES</title>
    <style>
        .ingroup {
            margin-top: 14px;
            color: black;
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
            position: absolute;
            left: 35%;
            top: 84%;
            border-radius: 8px;

        }

        #btn:hover {
            background-color: #f05262;
            border-color: #f05262;
            padding-left: 10px;

        }
        #form {
            position: absolute;
            left: 33%;
            top: 19%;
            background-color: #0cb4c9;
            border: 5px black;
            width: 500px;
            border-radius: 30px;
        }
        #form:hover {
            background-color: #30bacc;
        }

        #title {
            text-align: center;
            color: whitesmoke;
            font-family: Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;
        }


        .box {
            background-color: #fb8f04;
            color: #fff;
            border-radius: 15px;
            width: 350px;
            margin-left: 70px;
            margin-right: 50px;
            font-family: Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;
            font-size: 15px
        }

        .box:hover {
            background-color: #f59f30;
        }

        #btnbox {
            height: 3em;
        }
    </style>
    <script>
        function validate(form)
        {
            if(document.Stores.reg.value <= 0 ||document.Stores.reg.value > 999999)
            {
                    alert("Enter valid User ID");
                    document.Stores.reg.value="";
                    document.Stores.reg.focus();
                    return false;
            }
            if(isNaN(document.Stores.reg.value))
            {
                alert("User ID must be a number");
                document.Stores.reg.value="";
                document.Stores.reg.focus();
                return false;
            }
            if(document.Stores.amt.value < 0)
            {
                alert("Amount must be more than Zero");
                document.Stores.amt.value="";
                document.Stores.amt.focus();
                return false;
            }
            if(isNaN(document.Stores.amt.value))
            {
                alert("Amount must be a number");
                document.Stores.amt.value="";
                document.Stores.amt.focus();
                return false;
            }
            return true;
        }
    </script>
 </head>

 <body>
    <form name="Stores" onSubmit="return validate(this)" action="/action_page" method="post">
        <div id="form">
            <h1 id="title">Stores</h1>
            <br><br>
            <div class="box">
                &nbsp;User ID
                <input type="text" id="reg" name="regdnum" class="ingroup" width="30" required><br><br>
            </div>
            <br><br><br>
            <div class="box">
                &nbsp;Amount&nbsp;
                <input type="text" id="amt" name="amount" class="ingroup" required min="0"><br><br>
            </div>
            <br><br>
            <div id="btnbox">
                <input id="btn" type="submit" value="Authenticate" />
            </div>  
            <br>
        </div>
    </form>
 </body>
</html>

)=====";

const char RESULT_page[] = R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Success</title>
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
            <button id="btn">BACK</button>
    </div>
   </form>
</body>

</html>

)=====";

const char *ssid = "NETGEAR24";
const char *password = "omsrisairam";

ESP8266WebServer server(80);

void handleRoot()
{
    String s = STORES_page;           //Read HTML contents
    server.send(200, "text/html", s); //Send web page
}
void handleForm()
{
    String Id = server.arg("regdnum");
    String amount = server.arg("amount");
    String reg = Verify();
    Serial.print("REG_ID:");
    Serial.println(Id);
    if (Id == reg)
    {
        String s = RESULT_page;
        s.replace("@@@", "Rs "+amount + " deducted from " + Id);
        server.send(200, "text/html", s); //Send web page
    }
    else if (reg == "-10")
    {
        String s = RESULT_page;
        s.replace("@@@", "Time Expired");
        server.send(200, "text/html", s); //Send web page
    }
    else
    {
        String s = RESULT_page;
        s.replace("@@@", "Authentication Failed");
        server.send(200, "text/html", s); //Send web page
    }
}
void setup()
{
    Serial.begin(115200);
    fps.Open();
    fps.SetLED(true);
    WiFi.begin(ssid, password); //Connect to your WiFi router
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

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println("WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/action_page", handleForm);
    server.begin();
    Serial.println("HTTP server started");
}
void loop()
{
    server.handleClient();
}
