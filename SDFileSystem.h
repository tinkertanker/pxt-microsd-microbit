#ifndef MBED_SDFILESYSTEM_H
#define MBED_SDFILESYSTEM_H
#include "mbed.h"
#include "FATFileSystem.h"
#include <stdint.h>
class SDFileSystem : public FATFileSystem {
public:
    SDFileSystem(PinName mosi, PinName miso, PinName sclk, PinName cs, const char* name);
    virtual int disk_initialize();
    virtual int disk_status();
    virtual int disk_read(uint8_t* buffer, uint32_t block_number, uint32_t count);
    virtual int disk_write(const uint8_t* buffer, uint32_t block_number, uint32_t count);
    virtual int disk_sync();
    virtual uint32_t disk_sectors();

protected:

    int _cmd(int cmd, int arg);
    int _cmdx(int cmd, int arg);
    int _cmd8();
    int _cmd58();
    int initialise_card();
    int initialise_card_v1();
    int initialise_card_v2();

    int _read(uint8_t * buffer, uint32_t length);
    int _write(const uint8_t *buffer, uint32_t length);
    uint32_t _sd_sectors();
    uint32_t _sectors;

    void set_init_sck(uint32_t sck) { _init_sck = sck; }
    // Note: The highest SPI clock rate is 20 MHz for MMC and 25 MHz for SD
    void set_transfer_sck(uint32_t sck) { _transfer_sck = sck; }
    uint32_t _init_sck;
    uint32_t _transfer_sck;

    SPI _spi;
    DigitalOut _cs;
    int cdv;
    int _is_initialized;
};

#endif