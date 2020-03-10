/**
 * simple-router-info.ino
 *  René Vollmer
 *  Example code for the home-indicator-project [ https://www.instructables.com/id/Who-Is-Home-Indicator-aka-Weasley-Clock-Based-on-T ].
 *  
 * Please adjust your data below.
 *  
 * Created on: 10.03.2020
 *
 */

 
#if defined(ESP8266)
    //Imports for ESP8266
    #include <ESP8266WiFi.h>
    #include <ESP8266WiFiMulti.h>
    #include <ESP8266HTTPClient.h>
    ESP8266WiFiMulti WiFiMulti;
#elif defined(ESP32)
    //Imports for ESP32
    #include <WiFi.h>
    #include <WiFiMulti.h>
    #include <HTTPClient.h>
    WiFiMulti WiFiMulti;
#endif

#include <tr064.h>

//-------------------------------------------------------------------------------------
// Put your router settings here
//-------------------------------------------------------------------------------------

// Wifi network name (SSID)
const char *wifi_ssid = "WLANSID"; 

// Wifi network password
const char *wifi_password = "XXXXXXXXXXXXXXXXXXXXX";

// The username if you created an account, "admin" otherwise
const char *fuser = "homechecker";

// The password for the aforementioned account.
const char *fpass = "this_shouldBEaDecentPassword!";

// IP address of your router. This should be "192.168.179.1" for most FRITZ!Boxes
const char *IP = "192.168.179.1";

// Port of the API of your router. This should be 49000 for all TR-064 devices.
const int PORT = 49000;

//-------------------------------------------------------------------------------------

// TR-064 connection
TR064 connection(PORT, IP, fuser, fpass);

//###########################################################################################
//############################ OKAY, LET'S DO THIS! #########################################
//###########################################################################################

void setup() {
    // You might want to change the baud-rate
    Serial.begin(115200);
    if(Serial) Serial.setDebugOutput(true);

    // Clear some space in the serial monitor.
    if(Serial) {
        Serial.println();
        Serial.println();
        Serial.println();
    }

    // **************************************************
    // Wait a few secs for warm-up (dunno why, was in the default code for http connections).
    // You might be able to remove this block
    for (uint8_t t = 4; t > 0; t--) {
        if(Serial) Serial.printf("[SETUP] WAIT %d...\n", t);
        delay(1000);
        if(Serial) Serial.flush();
    }
    // **************************************************


    // Connect to wifi
    ensureWIFIConnection();

    // Initialize the TR-064 library
    // (IMPORTANT!)
    if(Serial) Serial.printf("Initialize TR-064 connection\n\n");
    // Available debug levels are:
    //  DEBUG_NONE, DEBUG_ERROR, DEBUG_WARNING, DEBUG_INFO, DEBUG_VERBOSE
    connection.debug_level = DEBUG_VERBOSE;
    connection.init();
}

void loop() {
    ensureWIFIConnection();
    
    if(Serial) Serial.println("-------------------------------------------");

    String params[][2] = {{}};
    String req[][2] = {{"NewModelName", ""}, {"NewSoftwareVersion", ""}, {"NewUpTime", ""}};
    connection.action("urn:dslforum-org:service:DeviceInfo:1", "GetInfo", params, 0, req, 3);

    if(Serial) Serial.println("[main] Info retrieved.");
    if(Serial) Serial.println("[main] Router Model: " + req[0][1]);
    if(Serial) Serial.println("[main] Router software version: " + req[1][1]);
    if(Serial) Serial.println("[main] Router up time: " + req[2][1] + "s");
    
    delay(5000);
}

/**
 * Makes sure there is a WIFI connection and waits until it is (re-)established.
 */
void ensureWIFIConnection() {
    if ((WiFiMulti.run() != WL_CONNECTED)) {
        WiFiMulti.addAP(wifi_ssid, wifi_password);
        WiFiMulti.run();
        while ((WiFiMulti.run() != WL_CONNECTED)) {
            delay(500);
        }
    }
}