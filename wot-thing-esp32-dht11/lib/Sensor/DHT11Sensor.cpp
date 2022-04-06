#include "DHT11Sensor.hpp"
#include "LoRaWAN.hpp"
#include "arduino_lmic.h"

cSensor mySensor {};

void
cSensor::setup(std::uint32_t uplinkPeriodMs) {
    m_dht.begin();

    // set the initial time.
    this->m_uplinkPeriodMs = uplinkPeriodMs;
    this->m_tReference = millis();

    // uplink right away
    this->m_fUplinkRequest = true;
}

void
cSensor::loop(void) {
    auto const tNow = millis();
    auto const deltaT = tNow - this->m_tReference;

    if (deltaT >= this->m_uplinkPeriodMs) {
        // request an uplink
        this->m_fUplinkRequest = true;

        // keep trigger time locked to uplinkPeriod
        auto const advance = deltaT / this->m_uplinkPeriodMs;
        this->m_tReference += advance * this->m_uplinkPeriodMs; 
    }

    // if an uplink was requested, do it.
    if (this->m_fUplinkRequest) {
        this->m_fUplinkRequest = false;
        this->doUplink();
    }
}

void
cSensor::doUplink(void) {
    // if busy uplinking, just skip
    if (this->m_fBusy)
        return;
    // if LMIC is busy, just skip
    if (LMIC.opmode & (OP_POLL | OP_TXDATA | OP_TXRXPEND))
        return;

    sensors_event_t event;
    m_dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
        Serial.println(F("Error reading temperature!"));
    }
    else {
        Serial.print(F("Temperature: "));
        Serial.print(event.temperature);
        Serial.println(F("°C"));
    }

    auto const t = event.temperature;

    // Get humidity event and print its value.
    m_dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
        Serial.println(F("Error reading humidity!"));
    }
    else {
        Serial.print(F("Humidity: "));
        Serial.print(event.relative_humidity);
        Serial.println(F("%"));
    }

    auto const uh = std::uint16_t(event.relative_humidity); // TODO uint8 should be enough?

    // format the uplink
    // big-endian.

    // TODO uint8 (0 - 255) would be enough for temperature and humidity??
    // DHT11 spec
    // Good for 20-80% humidity readings with 5% accuracy
    // Good for 0-50°C temperature readings ±2°C accuracy
    std::uint8_t uplink[4];
    auto const it = std::int16_t(floor(t + 0.5));

    uplink[0] = std::uint8_t(std::uint16_t(it) >> 8);
    uplink[1] = std::uint8_t(it);
    uplink[2] = std::uint8_t(uh >> 8);
    uplink[3] = std::uint8_t(uh);

    this->m_fBusy = true;
    
    if (! myLoRaWAN.SendBuffer(
        uplink, sizeof(uplink),
        // this is the completion function:
        [](void *pClientData, bool fSucccess) -> void {
            auto const pThis = (cSensor *)pClientData;
            pThis->m_fBusy = false;
        },
        (void *)this,
        /* confirmed */ false,
        /* port */ 1
        )) {
        // sending failed; callback has not been called and will not
        // be called. Reset busy flag.
        this->m_fBusy = false;
    }
}