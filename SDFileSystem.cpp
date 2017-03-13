#include "SDFileSystem.h"
#include "mbed_debug.h"

#define SD_COMMAND_TIMEOUT 5000

#define SD_DBG             0

SDFileSystem::SDFileSystem(PinName mosi, PinName miso, PinName sclk, PinName cs, const char* name) :
    FATFileSystem(name), _spi(mosi, miso, sclk), _cs(cs), _is_initialized(0) {
    _cs = 1;

    // Set default to 100kHz for initialisation and 1MHz for data transfer
    _init_sck = 100000;
    _transfer_sck = 1000000;
}

#define R1_IDLE_STATE           (1 << 0)
#define R1_ERASE_RESET          (1 << 1)
#define R1_ILLEGAL_COMMAND      (1 << 2)
#define R1_COM_CRC_ERROR        (1 << 3)
#define R1_ERASE_SEQUENCE_ERROR (1 << 4)
#define R1_ADDRESS_ERROR        (1 << 5)
#define R1_PARAMETER_ERROR      (1 << 6)

// Types
//  - v1.x Standard Capacity
//  - v2.x Standard Capacity
//  - v2.x High Capacity
//  - Not recognised as an SD Card
#define SDCARD_FAIL 0
#define SDCARD_V1   1
#define SDCARD_V2   2
#define SDCARD_V2HC 3

int SDFileSystem::initialise_card() {
    // Set to SCK for initialisation, and clock card with cs = 1
    _spi.frequency(_init_sck);
    _cs = 1;
    for (int i = 0; i < 16; i++) {
        _spi.write(0xFF);
    }

    // send CMD0, should return with all zeros except IDLE STATE set (bit 0)
    if (_cmd(0, 0) != R1_IDLE_STATE) {
        debug("No disk, or could not put SD card in to SPI idle state\n");
        return SDCARD_FAIL;
    }

    // send CMD8 to determine whther it is ver 2.x
    int r = _cmd8();
    if (r == R1_IDLE_STATE) {
        return initialise_card_v2();
    } else if (r == (R1_IDLE_STATE | R1_ILLEGAL_COMMAND)) {
        return initialise_card_v1();
    } else {
        debug("Not in idle state after sending CMD8 (not an SD card?)\n");
        return SDCARD_FAIL;
    }
}

int SDFileSystem::initialise_card_v1() {
    for (int i = 0; i < SD_COMMAND_TIMEOUT; i++) {
        _cmd(55, 0);
        if (_cmd(41, 0) == 0) {
            cdv = 512;
            debug_if(SD_DBG, "\n\rInit: SEDCARD_V1\n\r");
            return SDCARD_V1;
        }
    }

    debug("Timeout waiting for v1.x card\n");
    return SDCARD_FAIL;
}

int SDFileSystem::initialise_card_v2() {
    for (int i = 0; i < SD_COMMAND_TIMEOUT; i++) {
        wait_ms(50);
        _cmd58();
        _cmd(55, 0);
        if (_cmd(41, 0x40000000) == 0) {
            _cmd58();
            debug_if(SD_DBG, "\n\rInit: SDCARD_V2\n\r");
            cdv = 1;
            return SDCARD_V2;
        }
    }

    debug("Timeout waiting for v2.x card\n");
    return SDCARD_FAIL;
}

int SDFileSystem::disk_initialize() {
    _is_initialized = initialise_card();
    if (_is_initialized == 0) {
        debug("Fail to initialize card\n");
        return 1;
    }
    debug_if(SD_DBG, "init card = %d\n", _is_initialized);
    _sectors = _sd_sectors();

    // Set block length to 512 (CMD16)
    if (_cmd(16, 512) != 0) {
        debug("Set 512-byte block timed out\n");
        return 1;
    }

    // Set SCK for data transfer
    _spi.frequency(_transfer_sck);
    return 0;
}

int SDFileSystem::disk_write(const uint8_t* buffer, uint32_t block_number, uint32_t count) {
    if (!_is_initialized) {
        return -1;
    }
    
    for (uint32_t b = block_number; b < block_number + count; b++) {
        // set write address for single block (CMD24)
        if (_cmd(24, b * cdv) != 0) {
            return 1;
        }
        
        // send the data block
        _write(buffer, 512);
        buffer += 512;
    }
    
    return 0;
}

int SDFileSystem::disk_read(uint8_t* buffer, uint32_t block_number, uint32_t count) {
    if (!_is_initialized) {
        return -1;
    }
    
    for (uint32_t b = block_number; b < block_number + count; b++) {
        // set read address for single block (CMD17)
        if (_cmd(17, b * cdv) != 0) {
            return 1;
        }
        
        // receive the data
        _read(buffer, 512);
        buffer += 512;
    }

    return 0;
}

int SDFileSystem::disk_status() {
    // FATFileSystem::disk_status() returns 0 when initialized
    if (_is_initialized) {
        return 0;
    } else {
        return 1;
    }
}

int SDFileSystem::disk_sync() { return 0; }
uint32_t SDFileSystem::disk_sectors() { return _sectors; }


// PRIVATE FUNCTIONS
int SDFileSystem::_cmd(int cmd, int arg) {
    _cs = 0;

    // send a command
    _spi.write(0x40 | cmd);
    _spi.write(arg >> 24);
    _spi.write(arg >> 16);
    _spi.write(arg >> 8);
    _spi.write(arg >> 0);
    _spi.write(0x95);

    // wait for the repsonse (response[7] == 0)
    for (int i = 0; i < SD_COMMAND_TIMEOUT; i++) {
        int response = _spi.write(0xFF);
        if (!(response & 0x80)) {
            _cs = 1;
            _spi.write(0xFF);
            return response;
        }
    }
    _cs = 1;
    _spi.write(0xFF);
    return -1; // timeout
}
int SDFileSystem::_cmdx(int cmd, int arg) {
    _cs = 0;

    // send a command
    _spi.write(0x40 | cmd);
    _spi.write(arg >> 24);
    _spi.write(arg >> 16);
    _spi.write(arg >> 8);
    _spi.write(arg >> 0);
    _spi.write(0x95);

    // wait for the repsonse (response[7] == 0)
    for (int i = 0; i < SD_COMMAND_TIMEOUT; i++) {
        int response = _spi.write(0xFF);
        if (!(response & 0x80)) {
            return response;
        }
    }
    _cs = 1;
    _spi.write(0xFF);
    return -1; // timeout
}


int SDFileSystem::_cmd58() {
    _cs = 0;
    int arg = 0;

    // send a command
    _spi.write(0x40 | 58);
    _spi.write(arg >> 24);
    _spi.write(arg >> 16);
    _spi.write(arg >> 8);
    _spi.write(arg >> 0);
    _spi.write(0x95);

    // wait for the repsonse (response[7] == 0)
    for (int i = 0; i < SD_COMMAND_TIMEOUT; i++) {
        int response = _spi.write(0xFF);
        if (!(response & 0x80)) {
            int ocr = _spi.write(0xFF) << 24;
            ocr |= _spi.write(0xFF) << 16;
            ocr |= _spi.write(0xFF) << 8;
            ocr |= _spi.write(0xFF) << 0;
            _cs = 1;
            _spi.write(0xFF);
            return response;
        }
    }
    _cs = 1;
    _spi.write(0xFF);
    return -1; // timeout
}

int SDFileSystem::_cmd8() {
    _cs = 0;

    // send a command
    _spi.write(0x40 | 8); // CMD8
    _spi.write(0x00);     // reserved
    _spi.write(0x00);     // reserved
    _spi.write(0x01);     // 3.3v
    _spi.write(0xAA);     // check pattern
    _spi.write(0x87);     // crc

    // wait for the repsonse (response[7] == 0)
    for (int i = 0; i < SD_COMMAND_TIMEOUT * 1000; i++) {
        char response[5];
        response[0] = _spi.write(0xFF);
        if (!(response[0] & 0x80)) {
            for (int j = 1; j < 5; j++) {
                response[i] = _spi.write(0xFF);
            }
            _cs = 1;
            _spi.write(0xFF);
            return response[0];
        }
    }
    _cs = 1;
    _spi.write(0xFF);
    return -1; // timeout
}

int SDFileSystem::_read(uint8_t *buffer, uint32_t length) {
    _cs = 0;

    // read until start byte (0xFF)
    while (_spi.write(0xFF) != 0xFE);

    // read data
    for (uint32_t i = 0; i < length; i++) {
        buffer[i] = _spi.write(0xFF);
    }
    _spi.write(0xFF); // checksum
    _spi.write(0xFF);

    _cs = 1;
    _spi.write(0xFF);
    return 0;
}

int SDFileSystem::_write(const uint8_t*buffer, uint32_t length) {
    _cs = 0;

    // indicate start of block
    _spi.write(0xFE);

    // write the data
    for (uint32_t i = 0; i < length; i++) {
        _spi.write(buffer[i]);
    }

    // write the checksum
    _spi.write(0xFF);
    _spi.write(0xFF);

    // check the response token
    if ((_spi.write(0xFF) & 0x1F) != 0x05) {
        _cs = 1;
        _spi.write(0xFF);
        return 1;
    }

    // wait for write to finish
    while (_spi.write(0xFF) == 0);

    _cs = 1;
    _spi.write(0xFF);
    return 0;
}

static uint32_t ext_bits(unsigned char *data, int msb, int lsb) {
    uint32_t bits = 0;
    uint32_t size = 1 + msb - lsb;
    for (uint32_t i = 0; i < size; i++) {
        uint32_t position = lsb + i;
        uint32_t byte = 15 - (position >> 3);
        uint32_t bit = position & 0x7;
        uint32_t value = (data[byte] >> bit) & 1;
        bits |= value << i;
    }
    return bits;
}

uint32_t SDFileSystem::_sd_sectors() {
    uint32_t c_size, c_size_mult, read_bl_len;
    uint32_t block_len, mult, blocknr, capacity;
    uint32_t hc_c_size;
    uint32_t blocks;

    // CMD9, Response R2 (R1 byte + 16-byte block read)
    if (_cmdx(9, 0) != 0) {
        debug("Didn't get a response from the disk\n");
        return 0;
    }

    uint8_t csd[16];
    if (_read(csd, 16) != 0) {
        debug("Couldn't read csd response from disk\n");
        return 0;
    }

    // csd_structure : csd[127:126]
    // c_size        : csd[73:62]
    // c_size_mult   : csd[49:47]
    // read_bl_len   : csd[83:80] - the *maximum* read block length

    int csd_structure = ext_bits(csd, 127, 126);

    switch (csd_structure) {
        case 0:
            cdv = 512;
            c_size = ext_bits(csd, 73, 62);
            c_size_mult = ext_bits(csd, 49, 47);
            read_bl_len = ext_bits(csd, 83, 80);

            block_len = 1 << read_bl_len;
            mult = 1 << (c_size_mult + 2);
            blocknr = (c_size + 1) * mult;
            capacity = blocknr * block_len;
            blocks = capacity / 512;
            debug_if(SD_DBG, "\n\rSDCard\n\rc_size: %d \n\rcapacity: %ld \n\rsectors: %lld\n\r", c_size, capacity, blocks);
            break;

        case 1:
            cdv = 1;
            hc_c_size = ext_bits(csd, 63, 48);
            blocks = (hc_c_size+1)*1024;
            debug_if(SD_DBG, "\n\rSDHC Card \n\rhc_c_size: %d\n\rcapacity: %lld \n\rsectors: %lld\n\r", hc_c_size, blocks*512, blocks);
            break;

        default:
            debug("CSD struct unsupported\r\n");
            return 0;
    };
    return blocks;
}