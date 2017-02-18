#!/usr/bin/env python
#######################################################################
#   Title:          Doppelganger
#   File:           uploader8.py
#   Descr.:         Bootloader client for the Doppelganger
#   Stolen from:    Pinguino
#   Comment:
# #####################################################################
# This file is part of Doppelganger:
#       http://thewireddoesntexist.org/doppelganger
#       Released under the GPLV3 license (see LICENCE file)
# #####################################################################

# ----------------------------------------------------------------------
# Usage: uploader8.py path/filename.hex
# ----------------------------------------------------------------------

# This class is based on :
# Pinguino project uploader

# ----------------------------------------------------------------------
# Debug: export PYUSB_DEBUG=debug
# ----------------------------------------------------------------------

import sys
import os
import usb

# PyUSB Core module switch
# ----------------------------------------------------------------------

# 0=legacy, 1=core
PYUSB_USE_CORE = 1

# Globales
# ----------------------------------------------------------------------

# 8-bit Doppelganger's ID
# ----------------------------------------------------------------------

VENDOR_ID = 0x1807    # Doppelganger pseudo-License
PRODUCT_ID = 0x55AA    # Doppelganger Sub-License

# Hex format record types
# ----------------------------------------------------------------------

Data_Record = 0
End_Of_File_Record = 1
Extended_Segment_Address_Record = 2
Start_Segment_Address_Record = 3
Extended_Linear_Address_Record = 4
Start_Linear_Address_Record = 5

# usbBuf Data Packet Structure
# ----------------------------------------------------------------------
#    __________________
#    |    COMMAND     |   0 [BOOT_CMD]
#    |    LEN/SIZE    |   1 [BOOT_CMD_LEN]    or [BOOT_SIZE]
#    |     ADDRL      |   2 [BOOT_ADDR_LO]    or [BOOT_VER_MINOR]
#    |     ADDRH      |   3 [BOOT_ADDR_HI]    or [BOOT_VER_MAJOR ]
#    |     ADDRU      |   4 [BOOT_ADDR_UP]
#    |                |   5 [BOOT_DATA_START] or [BOOT_DEV1] or [BOOT_REV1]
#    |                |   6                      [BOOT_DEV2] or [BOOT_REV2]
#    .                .
#    .      DATA      .
#    .                .
#    |                |   62
#    |________________|   63
#
# ----------------------------------------------------------------------

BOOT_CMD = 0
BOOT_CMD_LEN = 1
BOOT_ADDR_LO = 2
BOOT_ADDR_HI = 3
BOOT_ADDR_UP = 4
BOOT_DATA_START = 5

BOOT_SIZE = 1

BOOT_VER_MINOR = 2
BOOT_VER_MAJOR = 3

BOOT_REV1 = 5
BOOT_REV2 = 6
BOOT_DEV1 = 7
BOOT_DEV2 = 8

# Bootloader commands
# ----------------------------------------------------------------------

READ_VERSION_CMD = 0x00
READ_FLASH_CMD = 0x01
WRITE_FLASH_CMD = 0x02
ERASE_FLASH_CMD = 0x03
# READ_EEDATA_CMD = 0x04
# WRITE_EEDATA_CMD = 0x05
# READ_CONFIG_CMD = 0x06
# WRITE_CONFIG_CMD = 0x07
RESET_CMD = 0xFF

# Block's size to write
# ----------------------------------------------------------------------

DATABLOCKSIZE = 32

# USB Max. Packet size
# ----------------------------------------------------------------------

MAXPACKETSIZE = 64

# Bulk endpoints
# ----------------------------------------------------------------------

IN_EP = 0x81    # endpoint for Bulk reads
OUT_EP = 0x01    # endpoint for Bulk writes

# Configuration
# ----------------------------------------------------------------------

ACTIVE_CONFIG = 0x01
INTERFACE_ID = 0x00
TIMEOUT = 10000

# Error codes returned by various functions
# ----------------------------------------------------------------------

ERR_NONE = 0
ERR_CMD_ARG = 1
ERR_CMD_UNKNOWN = 2
ERR_DEVICE_NOT_FOUND = 3
ERR_USB_INIT1 = 4
ERR_USB_INIT2 = 5
ERR_USB_OPEN = 6
ERR_USB_WRITE = 7
ERR_USB_READ = 8
ERR_HEX_OPEN = 9
ERR_HEX_STAT = 10
ERR_HEX_MMAP = 11
ERR_HEX_SYNTAX = 12
ERR_HEX_CHECKSUM = 13
ERR_HEX_RECORD = 14
ERR_VERIFY = 15
ERR_EOL = 16
ERR_USB_ERASE = 17

# Table with supported USB devices
# device_id:[PIC name, flash size(in bytes), eeprom size (in bytes)]
# ----------------------------------------------------------------------

devices_table = \
    {
        # 16F
        0x3020: ["16f1454", 0x02000, 0x00],
        0x3021: ["16f1455", 0x02000, 0x00],
        0x3023: ["16f1459", 0x02000, 0x00],
        0x3024: ["16lf1454", 0x02000, 0x00],
        0x3025: ["16lf1455", 0x02000, 0x00],
        0x3027: ["16lf1459", 0x02000, 0x00],

        # 18F
        0x4740: ["18f13k50", 0x02000, 0x80],
        0x4700: ["18lf13k50", 0x02000, 0x80],

        0x4760: ["18f14k50", 0x04000, 0xff],
        0x4720: ["18f14k50", 0x04000, 0xff],

        0x2420: ["18f2450", 0x04000, 0x00],
        0x1260: ["18f2455", 0x06000, 0xff],
        0x2a60: ["18f2458", 0x06000, 0xff],
        0x4c00: ["18f24j50", 0x04000, 0x00],
        0x4cc0: ["18lf24j50", 0x04000, 0x00],

        0x1240: ["18f2550", 0x08000, 0xff],
        0x2a40: ["18f2553", 0x08000, 0xff],
        0x4c20: ["18f25j50", 0x08000, 0x00],
        0x4ce0: ["18lf25j50", 0x08000, 0x00],
        0x5c20: ["18f25k50", 0x08000, 0xff],
        0x5ca0: ["18lf25k50", 0x08000, 0xff],

        0x4c40: ["18f26j50", 0x10000, 0x00],
        0x4d00: ["18lf26j50", 0x10000, 0x00],

        0x5860: ["18f27j53", 0x20000, 0x00],

        0x1200: ["18f4450", 0x04000, 0x00],
        0x1220: ["18f4455", 0x06000, 0x00],
        0x2a20: ["18f4458", 0x06000, 0xff],
        0x4c60: ["18f44j50", 0x04000, 0x00],
        0x4d20: ["18lf44j50", 0x04000, 0x00],

        0x1200: ["18f4550", 0x08000, 0xff],
        0x2a00: ["18f4553", 0x08000, 0xff],
        0x4c80: ["18f45j50", 0x08000, 0x00],
        0x4d40: ["18lf45j50", 0x08000, 0x00],
        0x5C00: ["18f45k50", 0x08000, 0xff],
        0x5C80: ["18lf45k50", 0x08000, 0xff],

        0x4ca0: ["18f46j50", 0x10000, 0x00],
        0x4d60: ["18f46j50", 0x10000, 0x00],

        0x58e0: ["18f47j53", 0x20000, 0x00],

        0x4100: ["18f65j50", 0x08000, 0x00],
        0x1560: ["18f66j50", 0x10000, 0x00],
        0x4160: ["18f66j55", 0x18000, 0x00],
        0x4180: ["18f67j50", 0x20000, 0x00],

        0x41a0: ["18f85j50", 0x08000, 0x00],
        0x41e0: ["18f86j50", 0x10000, 0x00],
        0x1f40: ["18f86j55", 0x18000, 0x00],
        0x4220: ["18f87j50", 0x20000, 0x00]
    }

# ----------------------------------------------------------------------


def getDevice(vendor, product):
    """ search USB device and returns a DeviceHandle object """

    if PYUSB_USE_CORE:
        device = usb.core.find(idVendor=vendor, idProduct=product)
        if device is None:
            return ERR_DEVICE_NOT_FOUND
        else:
            return device
    else:
        busses = usb.busses()
        for bus in busses:
            for device in bus.devices:
                if (device.idVendor, device.idProduct) == (vendor, product):
                    return device
        return ERR_DEVICE_NOT_FOUND


def initDevice(device):
    """ init Doppelganger device """

    if PYUSB_USE_CORE:
        if device.is_kernel_driver_active(INTERFACE_ID):
            try:
                device.detach_kernel_driver(INTERFACE_ID)
            except usb.core.USBError as e:
                msg = "Aborting: could not detach kernel driver: {0}"
                return(msg.format(str(e)))

        # The call to set_configuration must come before
        # claim_interface (which, btw, is optional).

        try:
            device.set_configuration(ACTIVE_CONFIG)
        except usb.core.USBError as e:
            msg = "Aborting: could not set configuration: {0}"
            return(msg.format(str(e)))

        try:
            usb.util.claim_interface(device, INTERFACE_ID)
        except usb.core.USBError as e:
            msg = "Aborting: could not claim interface: {0}"
            return(msg.format(str(e)))

        return device
    else:
        handle = device.open()
        if handle:
            try:
                handle.detachKernelDriver(INTERFACE_ID)
            except:
                pass
            try:
                handle.setConfiguration(ACTIVE_CONFIG)
            except:
                return("Aborting: could not set configuration")
            try:
                handle.claimInterface(INTERFACE_ID)
            except:
                pass
            return handle
        return ERR_USB_INIT1


def closeDevice(handle):
    """ Close currently-open USB device """

    if PYUSB_USE_CORE:
        usb.util.release_interface(handle, INTERFACE_ID)
    else:
        handle.releaseInterface()


def sendCommand(handle, usbBuf):
    """ send command to the bootloader """
    if PYUSB_USE_CORE:
        sent_bytes = handle.write(OUT_EP, usbBuf, TIMEOUT)
    else:
        sent_bytes = handle.bulkWrite(OUT_EP, usbBuf, TIMEOUT)

    if sent_bytes != len(usbBuf):
        return ERR_USB_WRITE

    if PYUSB_USE_CORE:
        return handle.read(IN_EP, MAXPACKETSIZE, TIMEOUT)
    else:
        return handle.bulkRead(IN_EP, MAXPACKETSIZE, TIMEOUT)


def resetDevice(handle):
    """ send reset command to the bootloader """
    usbBuf = [0] * MAXPACKETSIZE
    # command code
    usbBuf[BOOT_CMD] = RESET_CMD
    # write data packet
    if PYUSB_USE_CORE:
        handle.write(OUT_EP, usbBuf, TIMEOUT)
    else:
        handle.bulkWrite(OUT_EP, usbBuf, TIMEOUT)


def getVersion(handle):
    """ get bootloader version """
    usbBuf = [0] * MAXPACKETSIZE
    # command code
    usbBuf[BOOT_CMD] = READ_VERSION_CMD
    # write data packet and get response
    usbBuf = sendCommand(handle, usbBuf)
    if usbBuf == ERR_USB_WRITE:
        return ERR_USB_WRITE
    else:
        # major.minor
        return str(usbBuf[BOOT_VER_MAJOR]) + "." + str(usbBuf[BOOT_VER_MINOR])


def getDeviceID(handle, proc):
    """ read 2-byte device ID from
        PIC18F : 0x3FFFFE
        PIC16F : 0x8005        """

    if ("16f" in proc):
        # REVISION & DEVICE ID
        usbBuf = readFlash(handle, 0x8005, 4)
        if usbBuf == ERR_USB_WRITE or usbBuf is None:
            return ERR_USB_WRITE, ERR_USB_WRITE
        rev1 = usbBuf[BOOT_REV1]
        rev2 = usbBuf[BOOT_REV2]
        device_rev = (int(rev2) << 8) + int(rev1)
        dev1 = usbBuf[BOOT_DEV1]
        dev2 = usbBuf[BOOT_DEV2]
        device_id = (int(dev2) << 8) + int(dev1)

    else:
        # REVISION & DEVICE ID
        usbBuf = readFlash(handle, 0x3FFFFE, 2)
        print usbBuf
        if usbBuf == ERR_USB_WRITE or usbBuf is None:
            return ERR_USB_WRITE, ERR_USB_WRITE
        dev1 = usbBuf[BOOT_REV1]
        dev2 = usbBuf[BOOT_REV2]
        device_id = (int(dev2) << 8) + int(dev1)
        device_id = device_id & 0xFFE0
        device_rev = device_id & 0x001F

    return device_id, device_rev


def getDeviceFlash(device_id):
    """ get flash memory info """
    for n in devices_table:
        if n == device_id:
            return devices_table[n][1]
    return ERR_DEVICE_NOT_FOUND


def getDeviceName(device_id):
    """ get device chip name """
    for n in devices_table:
        if n == device_id:
            return devices_table[n][0]
    return ERR_DEVICE_NOT_FOUND


def eraseFlash(handle, address, numBlocks):
    """ erase n * 64- or 1024-byte blocks of flash memory """
    usbBuf = [0] * MAXPACKETSIZE
    # command code
    usbBuf[BOOT_CMD] = ERASE_FLASH_CMD
    # number of blocks to erase
    usbBuf[BOOT_SIZE] = numBlocks
    # 1rst block address
    # NB : must be divisible by 64 or 1024 depending on PIC model
    usbBuf[BOOT_ADDR_LO] = (address      ) & 0xFF
    usbBuf[BOOT_ADDR_HI] = (address >> 8 ) & 0xFF
    usbBuf[BOOT_ADDR_UP] = (address >> 16) & 0xFF
    # write data packet
    if PYUSB_USE_CORE:
        handle.write(OUT_EP, usbBuf, TIMEOUT)
    else:
        handle.bulkWrite(OUT_EP, usbBuf, TIMEOUT)


def readFlash(handle, address, length):
    """ read a block of flash """
    usbBuf = [0] * MAXPACKETSIZE
    # command code
    usbBuf[BOOT_CMD] = READ_FLASH_CMD
    # size of block
    usbBuf[BOOT_CMD_LEN] = length
    # address
    usbBuf[BOOT_ADDR_LO] = (address) & 0xFF
    usbBuf[BOOT_ADDR_HI] = (address >> 8) & 0xFF
    usbBuf[BOOT_ADDR_UP] = (address >> 16) & 0xFF
    # send request to the bootloader
    return sendCommand(handle, usbBuf)


def writeFlash(handle, address, datablock):
    """ write a block of code
        first 5 bytes are for block description
        (BOOT_CMD, BOOT_CMD_LEN and BOOT_ADDR)
        data block size should be of DATABLOCKSIZE bytes
        total length is then DATABLOCKSIZE + 5 < MAXPACKETSIZE """
    usbBuf = [0xFF] * MAXPACKETSIZE
    # command code
    usbBuf[BOOT_CMD] = WRITE_FLASH_CMD
    # size of block
    usbBuf[BOOT_CMD_LEN] = len(datablock)
    # block's address
    usbBuf[BOOT_ADDR_LO] = (address) & 0xFF
    usbBuf[BOOT_ADDR_HI] = (address >> 8) & 0xFF
    usbBuf[BOOT_ADDR_UP] = (address >> 16) & 0xFF
    # add data to the packet
    usbBuf[BOOT_DATA_START:] = datablock
    # write data packet on usb device
    if PYUSB_USE_CORE:
        handle.write(OUT_EP, usbBuf, TIMEOUT)
    else:
        handle.bulkWrite(OUT_EP, usbBuf, TIMEOUT)


def hexWrite(handle, filename, proc, memstart, memend):
    """     Parse the Hex File Format and send data to usb device

    [0]     Start code, one character, an ASCII colon ':'.
    [1:3]   Byte count, two hex digits.
    [3:7]   Address, four hex digits, a 16-bit address of the beginning
            of the memory position for the data. Limited to 64 kilobytes,
            the limit is worked around by specifying higher bits via
            additional record types. This address is big endian.
    [7:9]   Record type, two hex digits, 00 to 05, defining the type of
            the data field.
    [9:*]   Data, a sequence of n bytes of the data themselves,
            represented by 2n hex digits.
    [*:*]   Checksum, two hex digits - the least significant byte of the
            two's complement of the sum of the values of all fields
            except fields 1 and 6 (Start code ":" byte and two hex digits
            of the Checksum). It is calculated by adding together the
            hex-encoded bytes (hex digit pairs), then leaving only the
            least significant byte of the result, and making a 2's
            complement (either by subtracting the byte from 0x100,
            or inverting it by XOR-ing with 0xFF and adding 0x01).
            If you are not working with 8-bit variables,
            you must suppress the overflow by AND-ing the result with
            0xFF. The overflow may occur since both 0x100-0 and
            (0x00 XOR 0xFF)+1 equal 0x100. If the checksum is correctly
            calculated, adding all the bytes (the Byte count, both bytes
            in Address, the Record type, each Data byte and the Checksum)
            together will always result in a value wherein the least
            significant byte is zero (0x00).
            For example, on :0300300002337A1E
            03 + 00 + 30 + 00 + 02 + 33 + 7A = E2, 2's complement is 1E
    """
    # Addresses are doubled in the PIC16F HEX file
    if ("16f" in proc):
        memstart = memstart * 2
        memend = memend * 2

    data = []
    old_max_address = memstart
    old_min_address = memend
    max_address = 0
    min_address = 0
    address_Hi = 0
    codesize = 0

    erasedBlockSize = 64

    # image of the whole PIC memory (above memstart)
    # --------------------------------------------------------------

    for i in range(memend - memstart):
        data.append(0xFF)

    # read hex file
    # ------------------------------------------------------------------

    hexfile = open(filename, 'r')
    lines = hexfile.readlines()
    hexfile.close()

    # calculate checksum, code size and memmax
    # ------------------------------------------------------------------

    for line in lines:
        byte_count = int(line[1:3], 16)
        # lower 16 bits (bits 0-15) of the data address
        address_Lo = int(line[3:7], 16)
        record_type = int(line[7:9], 16)

        # checksum calculation (optional if speed is critical)
        end = 9 + byte_count * 2  # position of checksum at end of line
        checksum = int(line[end:end+2], 16)
        cs = 0
        i = 1
        while i < end:
            cs = cs + (0x100 - int(line[i:i+2], 16)) & 0xFF  # not(i)
            i = i + 2
        if checksum != cs:
            return ERR_HEX_CHECKSUM

        # extended linear address record
        if record_type == Extended_Linear_Address_Record:

            # upper 16 bits (bits 16-31) of the data address
            address_Hi = int(line[9:13], 16) << 16

        # data record
        elif record_type == Data_Record:

            # data's 32-bit address calculation
            address = address_Hi + address_Lo

            # min address
            if (address < old_min_address) and (address >= memstart):
                min_address = address
                old_min_address = address

            # max address
            if (address > old_max_address) and (address < memend):

                max_address = address + byte_count
                old_max_address = address

            if (address >= memstart) and (address < memend):
                # code size calculation
                codesize = codesize + byte_count

            # append data
            for i in range(byte_count):
                if ((address + i) < memend):
                    # Caution : addresses are not always contiguous
                    # data.append(int(line[9 + (2 * i) : 11 + (2 * i)], 16))
                    # data[address - memstart + i] = int(line[9 + (2 * i): 11 +
                    #                                            (2 * i)], 16)
                    data[address - min_address + i] = int(line[9 + (2 * i): 11
                                                               + (2 * i)], 16)

        # end of file record
        elif record_type == End_Of_File_Record:
            break

        # unsupported record type
        else:
            return ERR_HEX_RECORD

    # max_address must be divisible by erasedBlockSize
    # ------------------------------------------------------------------

    max_address = max_address + erasedBlockSize - (max_address % erasedBlockSize)
    if (max_address > memend):
        max_address = memend

    # erase memory from memstart to max_address
    # ------------------------------------------------------------------

    numBlocksMax = (memend - memstart) / erasedBlockSize
    numBlocks = (max_address - memstart) / erasedBlockSize

    if numBlocks > numBlocksMax:
        return ERR_USB_ERASE

    if numBlocks < 256:
        status = eraseFlash(handle, memstart, numBlocks)
        if status == ERR_USB_WRITE:
            return ERR_USB_WRITE

    else:
        numBlocks = numBlocks - 255
        upperAddress = memstart + 255 * erasedBlockSize
        # from self.board.memstart to upperAddress
        status = eraseFlash(handle, memstart, 255)
        if status == ERR_USB_WRITE:
            return ERR_USB_WRITE
        # erase flash memory from upperAddress to memmax
        status = eraseFlash(handle, upperAddress, numBlocks)
        if status == ERR_USB_WRITE:
            return ERR_USB_WRITE

    # write blocks of DATABLOCKSIZE bytes
    # ------------------------------------------------------------------

    for addr8 in range(min_address, max_address, DATABLOCKSIZE):
        index = addr8 - min_address
        # the addresses are doubled in the PIC16F HEX file
        if ("16f" in proc):
            addr16 = addr8 / 2
            status = writeFlash(handle, addr16,
                                data[index:index+DATABLOCKSIZE])
            if status == ERR_USB_WRITE:
                return ERR_USB_WRITE
        else:
            status = writeFlash(handle, addr8, data[index:index+DATABLOCKSIZE])
            if status == ERR_USB_WRITE:
                return ERR_USB_WRITE

    data[:] = []    # clear the list

    print("{0} bytes written".format(codesize))

    return ERR_NONE


def main(mcu, filename):
    # check file to upload
    # ------------------------------------------------------------------

    if filename == "":
        closeDevice(handle)
        return("Aborting: no program to write")

    hexfile = open(filename, 'r')
    if hexfile == "":
        return("Aborting: unable to open %s" % filename)

    hexfile.close()

    # search for a Doppelganger board
    # ------------------------------------------------------------------

    print("Looking for a Doppelganger board ...")
    device = getDevice(VENDOR_ID, PRODUCT_ID)
    if device == ERR_DEVICE_NOT_FOUND:
        return(("Aborting: Doppelganger not found. Is your device connected",
                  " and/or in bootloader mode ?"))
    else:
        print("Doppelganger found ...")

    handle = initDevice(device)
    if handle == ERR_USB_INIT1:
        print("... but upload is not possible.")
        print("Press the Reset button and try again.")
        return(0)

    # find out the processor
    # ------------------------------------------------------------------
    mcu = mcu.lower()
    device_id, device_rev = getDeviceID(handle, mcu)
    if device_id == ERR_USB_WRITE:
        closeDevice(handle)
        return("Aborting: unknown device ID")

    proc = getDeviceName(device_id)
    if proc == ERR_DEVICE_NOT_FOUND:
        closeDevice(handle)
        return("Aborting: unknown PIC (id=0x%X)" % device_id)

    elif proc != mcu:
        closeDevice(handle)
        msg = "Aborting: program compiled for %s but device has {0}"
        return(msg.format((mcu, proc)))

    else:
        msg = " - with PIC%s (id=0x%X, rev=%x)"
        print(msg % (proc, device_id, device_rev))

    # find out flash memory size
    # ------------------------------------------------------------------

    # lower limit of the flash memory (bootloader offset)
    # TODO : get it from the bootloader (cf. APPSTART)
    if ("16f" in proc):
        memstart = 0x800
    else:
        memstart = 0xC00

    # upper limit of the flash memory
    memend = getDeviceFlash(device_id)
    memfree = memend - memstart
    msg = " - with %d bytes free (%.2f/%d KB)"
    print(msg % (memfree, memfree/1024, memend/1024))
    msg = "   from 0x%05X to 0x%05X"
    print(msg % (memstart, memend))

    # find out bootloader version
    # ------------------------------------------------------------------
    print(" - with USB bootloader v%s" % getVersion(handle))

    # start writing
    # ------------------------------------------------------------------
    print("Uploading user program ...")
    status = hexWrite(handle, filename, proc, memstart, memend)

    if status == ERR_HEX_RECORD:
        closeDevice(handle)
        return("Aborting: record error")

    elif status == ERR_HEX_CHECKSUM:
        closeDevice(handle)
        return("Aborting: checksum error")

    elif status == ERR_USB_ERASE:
        print("Aborting: erase error")
        closeDevice(handle)
        return(0)

    elif status == ERR_NONE:
        print("%s successfully uploaded" % os.path.basename(filename))

    # reset and start start user's app.
    # ------------------------------------------------------------------

        resetDevice(handle)
        # Device can't be closed because it just has been reseted
        return("Starting user program ...")

    else:
        return("Aborting: unknown error")


if __name__ == "__main__":
    print("We use Python v%d.%d + PyUSB.%s" %
          (sys.version_info[0], sys.version_info[1],
           "core" if PYUSB_USE_CORE else "legacy"))
    if len(sys.argv) == 1:
        sys.exit(main('18f45k50', sys.argv[2]))
    else:
        sys.exit("Usage ex: uploader8.py tools/Blink1459.hex")
