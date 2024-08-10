#include "veml7700_Arduino.h"

#include "WireUtils.h"

#define VEML7700_CONFIG_REG_ADDR 0x0
#define VEML7700_THRESH_HI_REG_ADDR 0x1
#define VEML7700_THRESH_LO_REG_ADDR 0x2
#define VEML7700_PSCONFIG_REG_ADDR 0x3
#define VEML7700_INTEVT_REG_ADDR 0x6

#define VEML7700_CONFIG_GAIN_MASK     0b0001100000000000
#define VEML7700_CONFIG_IT_MASK       0b0000001111000000
#define VEML7700_CONFIG_PERS_MASK     0b0000000000110000
#define VEML7700_CONFIG_INT_MASK      0b0000000000000010
#define VEML7700_CONFIG_SHUTDOWN_MASK 0b0000000000000001
//#define VEML7700_CONFIG_ALL_MASK      0b0001101111110011  // all bits set except for reserved ones

#define VEML7700_CONFIG_GAIN_SHIFT     11
#define VEML7700_CONFIG_IT_SHIFT       6
#define VEML7700_CONFIG_PERS_SHIFT     4
#define VEML7700_CONFIG_INT_SHIFT      1
#define VEML7700_CONFIG_SHUTDOWN_SHIFT 0


#define VEML7700_PSCONFIG_PSM_MASK    0b110
#define VEML7700_PSCONFIG_PSM_EN_MASK 0b001

#define VEML7700_PSCONFIG_PSM_SHIFT    1
#define VEML7700_PSCONFIG_PSM_EN_SHIFT 0

VEML7700::VEML7700() {
    
}

uint8_t VEML7700::begin(TwoWire* wire_, const uint8_t device_addr_, bool wake_up) {
    device_addr = device_addr_;
    wire = wire_;

    if(wake_up)
        return this->wakeUp();

    return 0;
}

uint8_t VEML7700::wakeUp(void) {
    return this->write16(VEML7700_CONFIG_REG_ADDR, 0b0, VEML7700_CONFIG_SHUTDOWN_MASK);
}
uint8_t VEML7700::shutdown(void) {
    return this->write16(VEML7700_CONFIG_REG_ADDR, 0b1, VEML7700_CONFIG_SHUTDOWN_MASK);
}

uint8_t VEML7700::setHighThreshhold(uint16_t threshold) {
    return this->write16(VEML7700_THRESH_HI_REG_ADDR, threshold);
}
uint8_t VEML7700::setLowThreshhold(uint16_t threshold) {
    return this->write16(VEML7700_THRESH_LO_REG_ADDR, threshold);
}

void VEML7700::Config::setX(uint8_t value, uint16_t mask, uint8_t shift) {
    this->value &= ~mask;  // clear out old value
    this->value |= ((uint16_t)value << shift) & mask;
    this->mask |= mask;
}
uint8_t VEML7700::Config::getX(uint16_t mask, uint8_t shift) const {
    if((this->mask & mask) != mask) return -1;  // no value present

    return (this->value & mask) >> shift;
}

void VEML7700::Config::setGain(VEML7700::Gain_t value) {
    this->setX(value, VEML7700_CONFIG_GAIN_MASK, VEML7700_CONFIG_GAIN_SHIFT);
}
VEML7700::Gain_t VEML7700::Config::getGain(void) const {
    return this->getX(VEML7700_CONFIG_GAIN_MASK, VEML7700_CONFIG_GAIN_SHIFT);
}

void VEML7700::Config::setIT(VEML7700::IT_t value) {
    this->setX(value, VEML7700_CONFIG_IT_MASK, VEML7700_CONFIG_IT_SHIFT);
}
VEML7700::IT_t VEML7700::Config::getIT(void) const {
    return this->getX(VEML7700_CONFIG_IT_MASK, VEML7700_CONFIG_IT_SHIFT);
}

void VEML7700::Config::setPers(VEML7700::Pers_t value) {
    this->setX(value, VEML7700_CONFIG_PERS_MASK, VEML7700_CONFIG_PERS_SHIFT);
}
VEML7700::Pers_t VEML7700::Config::getPers(void) const {
    return this->getX(VEML7700_CONFIG_PERS_MASK, VEML7700_CONFIG_PERS_SHIFT);
}

void VEML7700::Config::setInt(VEML7700::Int_t value) {
    this->setX(value, VEML7700_CONFIG_INT_MASK, VEML7700_CONFIG_INT_SHIFT);
}
VEML7700::Int_t VEML7700::Config::getInt(void) const {
    return this->getX(VEML7700_CONFIG_INT_MASK, VEML7700_CONFIG_INT_SHIFT);
}

void VEML7700::Config::setSD(VEML7700::SD_t value) {
    this->setX(value, VEML7700_CONFIG_SHUTDOWN_MASK, VEML7700_CONFIG_SHUTDOWN_SHIFT);
}
VEML7700::SD_t VEML7700::Config::getSD(void) const {
    return this->getX(VEML7700_CONFIG_SHUTDOWN_MASK, VEML7700_CONFIG_SHUTDOWN_SHIFT) & 0b1;
}

uint8_t VEML7700::setConfig(VEML7700::Config value) {
    return this->write16(VEML7700_CONFIG_REG_ADDR, value.value, value.mask);
}


void VEML7700::PsConfig::setX(uint8_t value, uint8_t mask, uint8_t shift) {
    this->value &= ~mask;  // clear out old value
    this->value |= (value << shift) & mask;
    this->mask |= mask;
}
uint8_t VEML7700::PsConfig::getX(uint8_t mask, uint8_t shift) const {
    if((this->mask & mask) != mask) return -1;  // no value present

    return (this->value & mask) >> shift;
}

void VEML7700::PsConfig::setPSM(VEML7700::PSM_t value) {
    this->setX(value, VEML7700_PSCONFIG_PSM_MASK, VEML7700_PSCONFIG_PSM_SHIFT);
}
VEML7700::PSM_t VEML7700::PsConfig::getPSM(void) const {
    return this->getX(VEML7700_PSCONFIG_PSM_MASK, VEML7700_PSCONFIG_PSM_SHIFT);
}

void VEML7700::PsConfig::setPSM_EN(VEML7700::PSM_EN_t value) {
    this->setX(value, VEML7700_PSCONFIG_PSM_EN_MASK, VEML7700_PSCONFIG_PSM_EN_SHIFT);
}
VEML7700::PSM_EN_t VEML7700::PsConfig::getPSM_EN(void) const {
    return this->getX(VEML7700_PSCONFIG_PSM_EN_MASK, VEML7700_PSCONFIG_PSM_EN_SHIFT);
}


uint8_t VEML7700::setPsConfig(VEML7700::PsConfig value) {
    return this->write16(VEML7700_PSCONFIG_REG_ADDR, value.value, value.mask);
}

uint16_t VEML7700::getChannelValue(VEML7700::channel_t channel, uint8_t* error) {
    if(channel != CHANNEL_WHITE && channel != CHANNEL_ALS) {
        if(error != NULL) *error = 8;  // invalid channel number error
        return 0;
    }

    return this->read16(channel, error);
}

VEML7700::IntEvt_t VEML7700::getIntEvt(uint8_t* error) {
    return this->read16(VEML7700_INTEVT_REG_ADDR, error);
}

// helpers

uint16_t VEML7700::read16(const uint8_t register_address, uint8_t* error) {
    return WireUtils::read16(wire, device_addr, register_address, error);
}
uint8_t VEML7700::write16(const uint8_t register_address, const uint16_t data, uint16_t mask) {
    return WireUtils::write16(wire, device_addr, register_address, data, mask);
}