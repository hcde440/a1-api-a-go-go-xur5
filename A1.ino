// HCDE 440 A1:  API-a-go-go
// Melody Xu
// 4/9/19

#include "arduino_secrets.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> //provides the ability to parse and construct JSON objects

String ssid = H_WIFI;
String pass = H_PASS;

String weatherKey = MET_ID;
String china = "CHN";


typedef struct { //here we create a new data type definition, a box to hold other data types
  String area; // land area in 2012
  String lang; // languages spoken
  String incm; // world bank income group
  String capt; // capital city
  String lon; // longitude
  String lat; // latitude
  
} CountryData;     //then we give our new data structure a name so we can use it in our code
CountryData stat; //we have created a MetData type, but not an instance of that type,
                  //so we create the variable 'condition' of type GeoData
                 
void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));

  // Connecting to WiFi
  Serial.print("Connecting to "); Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(); Serial.println("WiFi connected"); Serial.println();
  Serial.print("Your ESP has been assigned the internal IP address ");
  Serial.println(WiFi.localIP());
  Serial.println("");
  Serial.println("____________________________________________________");
  Serial.println("Below are the country stats you requested for China:");
  Serial.println("");
  getStat(china); // Get China Stats
  getStat2("cn"); // Get China Stats
}

void loop() {
}

// Data from WHO
void getStat(String code) {
  HTTPClient theClient; // Creating the client for calling the API
  String apiCall = "http://apps.who.int/gho/athena/api/GHO/WHOSIS_000001?filter=COUNTRY:";  
  apiCall += code;
  apiCall += ";YEAR:2011;SEX:BTSX"; // have more filters to cut down data size
  apiCall += "&format=json";
  theClient.begin(apiCall); // Calling API using the URL
  int httpCode = theClient.GET(); // Getting http code
  if (httpCode > 0) {

    if (httpCode == HTTP_CODE_OK) {             // If the code is 200
      String payload = theClient.getString(); // Getting the string of information
      DynamicJsonBuffer jsonBuffer;                       // Storing JSON
      JsonObject& root = jsonBuffer.parseObject(payload); // Converting into JSON
      if (!root.success()) {                              // If parsing failed
        Serial.println("parseObject() failed in getStat()."); 
        return;
      }
      // Below is accessing the JSON library for Land Area, Languages, and Income Group information
      stat.area = root["dimension"][4]["code"][0]["attr"][2]["value"].as<String>(); 
      Serial.println("Land Area: " + stat.area);
      stat.lang = root["dimension"][4]["code"][0]["attr"][3]["value"].as<String>(); 
      Serial.println("Languages: " + stat.lang);
      stat.incm = root["dimension"][4]["code"][0]["attr"][8]["value"].as<String>(); 
      Serial.println("Income Group: " + stat.incm);
    }
  }
  else {
    Serial.printf("Something went wrong with connecting to the endpoint.");
  }
}

// Data from world bank
void getStat2(String code) {
  HTTPClient theClient; // Creating the client for calling the API
  String apiCall = "http://api.worldbank.org/v2/country/";
  apiCall += code;
  apiCall += "?format=json";  
  theClient.begin(apiCall); // Calling API using the URL
  int httpCode = theClient.GET(); // Getting http code
  if (httpCode > 0) {

    if (httpCode == HTTP_CODE_OK) {             // If the code is 200
      String payload = theClient.getString(); // Getting the string of information
      int leng = payload.length();
      payload = payload.substring(49, leng - 2); // Cutting down the string to include only useful parts,
                                                //  which was able to be put into the JSON Buffer without causing any errors
      DynamicJsonBuffer jsonBuffer;                       // Storing JSON
      JsonObject& root = jsonBuffer.parseObject(payload); // Converting into JSON
      if (!root.success()) {                              // If parsing failed
        Serial.println("parseObject() failed in getStat()."); 
        return;
      }
      // Below is accessing the JSON library for Capital City, Longitude, and Latitude information
      stat.capt = root["capitalCity"].as<String>(); 
      Serial.println("Capital City: " + stat.capt);
      stat.lon = root["longitude"].as<String>(); 
      Serial.println("Longitude: " + stat.lon);
      stat.lat = root["latitude"].as<String>(); 
      Serial.println("Latitude: " + stat.lat);
    }
  }
  else {
    Serial.printf("Something went wrong with connecting to the endpoint.");
  }
}

