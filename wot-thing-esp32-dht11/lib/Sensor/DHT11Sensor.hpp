#ifndef __DHT11SENSOR_H__
#define __DHT11SENSOR_H__

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "App.hpp"

class cSensor {
public:
    cSensor() {};

    void setup(std::uint32_t uplinkPeriodMs = 6 * 60 * 1000);

    void loop();

private:
    void doUplink();

    bool m_fUplinkRequest;              // set true when uplink is requested
    bool m_fBusy;                       // set true while sending an uplink
    std::uint32_t m_uplinkPeriodMs;     // uplink period in milliseconds
    std::uint32_t m_tReference;         // time of last uplink

    //   The temperature/humidity sensor
    DHT_Unified m_dht = DHT_Unified(DHT_PIN, DHT_TYPE);
};

extern cSensor mySensor;

#endif