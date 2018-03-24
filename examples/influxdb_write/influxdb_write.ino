
#include <Arduino.h>

#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WiFiMulti.h>
#endif

#include <ESPinfluxdb.h>

const char *INFLUXDB_HOST = "host_IP";
const uint16_t INFLUXDB_PORT = 8086;

const char *DATABASE = "dbname";
const char *DB_USER = "dbuser";
const char *DB_PASSWORD = "dbpassword";


#if defined(ESP8266)
ESP8266WiFiMulti WiFiMulti;
#elif defined(ESP32)
WiFiMulti WiFiMulti;
#endif


Influxdb influxdb(INFLUXDB_HOST, INFLUXDB_PORT);

void setup() {
        Serial.begin(115200);
        WiFiMulti.addAP("SSID", "password");
        while (WiFiMulti.run() != WL_CONNECTED) {
                delay(100);
        }
        Serial.println("Ready");


        //influxdb.opendb(DATABASE, DB_USER, DB_PASSWORD);
        while (influxdb.opendb(DATABASE)!=DB_SUCCESS) {
                Serial.println("Opend database failed");
                delay(10000);
        }

}

void loop() {
        // Writing data with influxdb HTTP API
        // https://docs.influxdata.com/influxdb/v1.5/guides/writing_data/
        // Serial.println("Writing data to host " + String(INFLUXDB_HOST) + ":" +
        //                INFLUXDB_PORT + "'s database=" + DATABASE);
        //
        // String data = "analog_read,method=HTTP_API,pin=A0 value=" + String(analogRead(A0));
        // influxdb.write(data);
        //
        // Serial.println(influxdb.response() == DB_SUCCESS ? "HTTP write success"
        //                : "Writing failed");


        // Create data object with measurment name=analog_read
        dbMeasurement row("analog_read");
        row.addTag("testTag", "Tag1"); // Add tag
        row.addTag("pin", "A0"); // Add pin tag
        row.addField("ADC", analogRead(A0)); // Add value field
        row.addField("random", random(100)); // Add random value

        Serial.println(influxdb.write(row) == DB_SUCCESS ? "Object write success"
                       : "Writing failed");

        // Empty field object.
        row.empty();

        // Querying Data
        Serial.println("Querying data ........");

        // https://docs.influxdata.com/influxdb/v1.5/query_language/
        String influxQL = "select * from analog_read order by time desc limit 2";
        if (influxdb.query(influxQL) != DB_SUCCESS)
                Serial.println("Query Failed");

        delay(30000);
}
