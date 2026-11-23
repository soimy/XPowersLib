#include <cassert>
#include <cstdio>
#include <cstring>

#include "XPowersAXP202.hpp"

static uint8_t registers[256];
static bool read_fails;
static unsigned writes;

static int read_register(uint8_t, uint8_t reg, uint8_t *data, uint8_t length)
{
    if (read_fails) return -1;
    std::memcpy(data, registers + reg, length);
    return 0;
}

static int write_register(uint8_t, uint8_t reg, uint8_t *data, uint8_t length)
{
    std::memcpy(registers + reg, data, length);
    ++writes;
    return 0;
}

int main()
{
    XPowersAXP202 power(AXP202_SLAVE_ADDRESS, read_register, write_register);
    registers[0x33] = 0xF7;
    power.setChargerTerminationCurr(XPOWERS_AXP202_CHG_ITERM_LESS_10_PERCENT);
    assert((registers[0x33] & 0x0F) == 7); // 1000 mA must not become 900 mA.
    assert(registers[0x33] == 0xE7);

    for (unsigned value = 0; value <= 0xFF; ++value) {
        for (unsigned option = 0; option <= 1; ++option) {
            registers[0x33] = static_cast<uint8_t>(value);
            unsigned before = writes;
            power.setChargerTerminationCurr(static_cast<xpowers_axp202_chg_iterm_t>(option));
            unsigned expected = option ? value | 0x10 : value & ~0x10u;
            assert(registers[0x33] == expected);
            assert(power.getChargerTerminationCurr() == option);
            assert(writes == before + 1);
        }
    }

    registers[0x33] = 0xA7;
    unsigned before = writes;
    power.setChargerTerminationCurr(static_cast<xpowers_axp202_chg_iterm_t>(0xFF));
    assert(registers[0x33] == 0xA7 && writes == before);
    read_fails = true;
    power.setChargerTerminationCurr(XPOWERS_AXP202_CHG_ITERM_LESS_10_PERCENT);
    power.setChargerTerminationCurr(XPOWERS_AXP202_CHG_ITERM_LESS_15_PERCENT);
    assert(registers[0x33] == 0xA7 && writes == before);
    std::puts("AXP202 termination: 512 register cases, invalid option and read failures passed");
}
