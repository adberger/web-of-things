#ifndef __LORAWAN_H__
#define __LORAWAN_H__

#include <Arduino_LoRaWAN_network.h>

class cMyLoRaWAN : public Arduino_LoRaWAN_network {
public:
    cMyLoRaWAN() {};
    using Super = Arduino_LoRaWAN_network;
    void setup(const Arduino_LMIC::HalPinmap_t * pPinmap);

protected:
    // you'll need to provide implementation for this.
    virtual bool GetOtaaProvisioningInfo(Arduino_LoRaWAN::OtaaProvisioningInfo*) override;
    // if you have persistent storage, you can provide implementations for these:
    virtual void NetSaveSessionInfo(const SessionInfo &Info, const uint8_t *pExtraInfo, size_t nExtraInfo) override;
    virtual void NetSaveSessionState(const SessionState &State) override;
    virtual bool NetGetSessionState(SessionState &State) override;
};

extern cMyLoRaWAN myLoRaWAN;
extern const cMyLoRaWAN::lmic_pinmap myPinMap;

#endif