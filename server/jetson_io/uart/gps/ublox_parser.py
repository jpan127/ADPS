from enum import Enum, unique
import serial
from ubx_packet import UbxPacket
from nmea_packet import NmeaPacket



_UART_PORT = "/dev/ttyACM0"
_UART_BAUD_RATE = 9600

# https://www.u-blox.com/sites/default/files/products/documents/u-blox6_ReceiverDescrProtSpec_%28GPS.G6-SW-10018%29_Public.pdf
class UbloxParser:
    """
    Parses packet / frames outputted from a Ublox GPS
    """

    # Special characters that mark the beginning of a UBX or NMEA packet
    # Useful for synchronizing the parser to the start of a frame
    SYNC_KEYS = {
        "UBX1" : 181,       # First  UBX  sync key
        "UBX2" : 98,        # Second UBX  sync key
        "NMEA" : ord('$'),  # Only   NMEA sync key
    }

    @unique
    class SynchronizationState(Enum):
        """
        Enumerates the state of the synchronizer
        """
        UNSYNCED    = 0
        UBX_KEY1    = 1
        UBX_KEY2    = 2
        NMEA_KEY    = 3
        SYNCED_UBX  = 4
        SYNCED_NMEA = 5

    def __init__(self):
        """
        Initializer
        """
        self.ubx_packet = None
        self.nmea_packet = None
        self.sync_state = None
        self.resync_error_counter = None
        self._reset()

    def _reset(self):
        """
        Resets the member variables : the packets, the synchronizer
        """
        self.ubx_packet  = UbxPacket()
        self.nmea_packet = NmeaPacket()
        self.sync_state  = self.SynchronizationState.UNSYNCED
        self.resync_error_counter = 0

    def synchronize(self, byte):
        """
        Synchronizes with the start of the next UBX or NMEA packet
        :param byte : Current parsed byte to check
        :returns    : True if currently synced, false if not
        """
        if self.sync_state is self.SynchronizationState.UNSYNCED:
            if byte == self.SYNC_KEYS["UBX1"]:
                self.sync_state = self.SynchronizationState.UBX_KEY1
            elif byte == self.SYNC_KEYS["NMEA"]:
                self.sync_state = self.SynchronizationState.NMEA_KEY
            else:
                self.resync_error_counter += 1
        elif self.sync_state is self.SynchronizationState.UBX_KEY1:
            if byte == self.SYNC_KEYS["UBX2"]:
                self.sync_state = self.SynchronizationState.UBX_KEY2
            else:
                self.sync_state = self.SynchronizationState.UNSYNCED
                self.resync_error_counter += 1
        elif self.sync_state is self.SynchronizationState.UBX_KEY2:
            self.sync_state = self.SynchronizationState.SYNCED_UBX
        elif self.sync_state is self.SynchronizationState.NMEA_KEY:
            self.sync_state = self.SynchronizationState.SYNCED_NMEA

        return self.sync_state in [self.SynchronizationState.SYNCED_UBX, self.SynchronizationState.SYNCED_NMEA]

    def parse(self, byte):
        """
        Parses a byte from the Ublox serial output
        Synchronizes if the state is unsynced
        Otherwise, will bubble the byte down to the current packet to be parsed
        If a parser error is detected, reset and resync to the next packet
        :param byte : Current parsed byte to check
        """
        # Make sure parser is currently synchronized
        if self.synchronize(byte):
            if self.sync_state is self.SynchronizationState.SYNCED_UBX:
                parse_status = self.ubx_packet.parse(byte)
                if parse_status is None:
                    print("[UBX] Parser error")
                    self._reset()
                elif parse_status is True:
                    self._reset()
            elif self.sync_state is self.SynchronizationState.SYNCED_NMEA:
                parse_status = self.nmea_packet.parse(byte)
                if parse_status is None:
                    print("[NMEA] Parser error")
                    self._reset()
                elif parse_status is True:
                    self._reset()


if __name__ == '__main__':
    try:
        with serial.Serial(_UART_PORT, _UART_BAUD_RATE) as port:
            parser = UbloxParser()
            while True:
                line = port.read(100)
                if line:
                    # Converts to hex string
                    line = line.hex()
                    # Splits string into a list of byte strings
                    line = [line[i:i+2] for i in range(0, len(line), 2)]
                    for b in line:
                        b = int(b, 16)
                        parser.parse(b)
    except KeyboardInterrupt:
        pass
