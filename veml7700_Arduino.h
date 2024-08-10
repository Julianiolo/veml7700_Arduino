#ifndef __VEML7700_H__
#define __VEML7700_H__

#include <Arduino.h>
#include <Wire.h>

// https://www.vishay.com/docs/84286/veml7700.pdf

#define VEML7700_VERSION "1.0.0"
#define VEML7700_VERSION_MAJOR 1
#define VEML7700_VERSION_MINOR 0
#define VEML7700_VERSION_PATCH 0

#define VEML7700_DEF_ADDR 0x10

class VEML7700 {
private:
    uint8_t device_addr = 0; // i2c address of the device
    TwoWire* wire = NULL;   // i2c interface
public:

    VEML7700();

    // returns same errors as wakeUp() function
    uint8_t begin(TwoWire* wire = &Wire, const uint8_t device_addr = VEML7700_DEF_ADDR, bool wake_up = false);

    // returns same errors as WireUtils::write16Confirm
    uint8_t wakeUp(void);
    // returns same errors as WireUtils::write16Confirm
    uint8_t shutdown(void);

    uint8_t setHighThreshhold(uint16_t threshhold);
    uint8_t setLowThreshhold(uint16_t threshhold);

    // Gain
    enum {
        Gain_1x     = 0, // x 1
        Gain_2x     = 1, // x 2
        Gain_0_25x  = 2, // x 1/4
        Gain_0_125x = 3  // x 1/8
    };
    typedef uint8_t Gain_t;

    // Integration time
    enum {
        IT_25ms  = 0b1100,
        IT_50ms  = 0b1000,
        IT_100ms = 0b0000,
        IT_200ms = 0b0001,
        IT_400ms = 0b0010,
        IT_800ms = 0b0011,
    };
    typedef uint8_t IT_t;

    // Persistence protect number setting
    enum {
        Pers_1 = 0,
        Pers_2 = 1,
        Pers_4 = 2,
        Pers_8 = 3,
    };
    typedef uint8_t Pers_t;

    // Interrupt enable
    enum {
        Int_Disabled = 0, // interrupt disabled
        Int_Enabled  = 1, // interrupt enabled
    };
    typedef uint8_t Int_t;

    // Shutdown/Awake
    enum {
        SD_Awake    = 0, 
        SD_Shutdown = 1,  // (default)
    };
    typedef uint8_t SD_t;

    // this class allows writing a complete config to the device at once (since everything is in the same register)
    class Config {
    public:
        uint16_t value; // dont use unless you know what you're doing
        uint16_t mask;  // dont use unless you know what you're doing

        /*
            all get* functions return (uint8_t)-1 (so 255) if no value is present (= mask is not set for this value)
        */

        // Gain
        void setGain(VEML7700::Gain_t value);
        VEML7700::Gain_t getGain(void) const;

        // Integration time
        void setIT(VEML7700::IT_t value);
        VEML7700::IT_t getIT(void) const;

        // Persistence protect number setting
        void setPers(VEML7700::Pers_t value);
        VEML7700::Pers_t getPers(void) const;

        // Interrupt
        void setInt(VEML7700::Int_t value);
        VEML7700::Int_t getInt(void) const;

        // Shutdown/Awake
        void setSD(VEML7700::SD_t value);
        VEML7700::SD_t getSD(void) const;

    protected:
        // helpers
        void setX(uint8_t value, uint16_t mask, uint8_t shift);
        uint8_t getX(uint16_t mask, uint8_t shift) const;
    };

    // returns the same errors as write16
    uint8_t setConfig(VEML7700::Config value);
    // there is no getConfig, since datasheet specifies it to be write only

    // Power saving mode
    enum {
        PSM_Mode1 = 0, 
        PSM_Mode2 = 1, 
        PSM_Mode3 = 2, 
        PSM_Mode4 = 3, 
    };
    typedef uint8_t PSM_t;

    // Power saving mode enable (PSM_EN)
    enum {
        PSM_EN_Disabled = 0, // interrupt disabled
        PSM_EN_Enabled  = 1, // interrupt enabled
    };
    typedef uint8_t PSM_EN_t;

    // Power saving config
    class PsConfig {
    public:
        uint8_t value; // dont use unless you know what you're doing
        uint8_t mask;  // dont use unless you know what you're doing

        /*
            all get* functions return (uint8_t)-1 (so 255) if no value is present (= mask is not set for this value)
        */

        // Power saving mode
        void setPSM(VEML7700::PSM_t value);
        VEML7700::PSM_t getPSM(void) const;

        // Power saving mode enable (PSM_EN)
        void setPSM_EN(VEML7700::PSM_EN_t value);
        VEML7700::PSM_EN_t getPSM_EN(void) const;

    protected:
        // helpers
        void setX(uint8_t value, uint8_t mask, uint8_t shift);
        uint8_t getX(uint8_t mask, uint8_t shift) const;
    };

    // returns the same errors as write16
    uint8_t setPsConfig(VEML7700::PsConfig value);
    // there is no getPsConfig, since datasheet specifies it to be write only

    enum {
        CHANNEL_WHITE = 0x4,
        CHANNEL_ALS   = 0x5,
    };
    typedef uint8_t channel_t;

    // returns same errors as WireUtils::read16 + 8: invalid channel number
    uint16_t getChannelValue(VEML7700::channel_t channel, uint8_t* error = NULL);

    enum {
        INTEVT_HIGH_THRESHOLD = 1<<14,
        INTEVT_LOW_THRESHOLD  = 1<<15,
    };
    typedef uint16_t IntEvt_t;

    VEML7700::IntEvt_t getIntEvt(uint8_t* error = NULL);

private:
    uint16_t read16(const uint8_t register_address, uint8_t* error = NULL);
    uint8_t write16(const uint8_t register_address, const uint16_t data, uint16_t mask = 0xffff);
};

#endif