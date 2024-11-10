#ifndef LORAWAN_H
#define LORAWAN_H

#include <RadioLib.h>
#include <cstdint>
#include <esp_attr.h>
#include <functional>
#include <string>

namespace GAIT {

    // utilities & vars to support ESP32 deep-sleep. The RTC_DATA_ATTR attribute
    // puts these in to the RTC memory which is preserved during deep-sleep
    extern RTC_DATA_ATTR uint16_t bootCountSinceUnsuccessfulJoin;
    extern RTC_DATA_ATTR uint8_t session[];

    template <typename LoRaModule>
    class LoRaWAN {
    public:
        LoRaWAN(const LoRaWANBand_t& region,
                const uint64_t joinEUI,
                const uint64_t devEUI,
                uint8_t appKey[16],
                uint8_t nwkKey[16],
                uint8_t pin1,
                uint8_t pin2,
                uint8_t pin3,
                uint8_t pin4,
                const uint8_t subBand = 0);

        void goToSleep();

        void setup(uint16_t bootCount);

        void setUplinkPayload(uint8_t fPort, const std::string& uplinkPayload);
        void setDownlinkCB(std::function<void(uint8_t, uint8_t*, std::size_t)> downlinkCB);

        void loop();

    private:
        int16_t activate(uint16_t bootCount);

        std::function<void(uint8_t fPort, uint8_t*, std::size_t)> downlinkCB;

        LoRaModule radio;
        LoRaWANNode node;

        // reserved for mac commands: 0
        // Here for application use: 1 ... 219,
        // Here 220 (request for further downlinks)
        // Here 221 (info), 222 (warning), 223 (error) are used
        // reserved for further use: 224 ... 255,
        uint8_t fPort = 221;
        std::string uplinkPayload;
    };

} // namespace GAIT

#endif // LORAWAN_H
