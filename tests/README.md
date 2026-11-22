# Host regression tests

Run from the repository root on Linux or macOS with a C++ compiler:

```sh
mkdir -p build
c++ -std=c++11 -Dlinux -Isrc tests/axp202_charge_termination.cpp src/XPowersLibInterface.cpp -o build/axp202-charge-termination-test
./build/axp202-charge-termination-test
```

The register callbacks replace only the I2C boundary. The production AXP202 setter
must select 10%/15% termination using REG33H bit 4 and preserve every other bit,
including the constant-current setting in bits 3:0. See REG33H in the bundled
AXP202 datasheet. The test also checks getter consistency and no writes for an
invalid option or failed register read.
