from enum import Enum, unique
from ublox_packet import UbloxPacket



class UbxPacket(UbloxPacket):
    """
    Packet that follows the UBX protocol frame format
    :documentation : Page 96
    """

    @unique
    class ParserState(Enum):
        """
        Enumerates the current state of the parser
        """
        MESSAGE_CLASS = 0
        ID            = 1
        LENGTH_1      = 2
        LENGTH_2      = 3
        PAYLOAD       = 4
        CHECKSUM_A    = 5
        CHECKSUM_B    = 6

    def __init__(self):
        """
        Initializer
        """
        self.msg_class    = 0
        self.id           = 0
        self.length       = 0
        self.payload      = []
        self.checksum_a   = 0
        self.checksum_b   = 0
        self.parser_state = self.ParserState.MESSAGE_CLASS

    def parse(self, byte):
        """
        Determines where the current byte fits into the packet
        :param byte : Current parsed byte to check
        :returns    : True  if packet is done parsing
                      False if packet is not done yet
                      None  if there was an error parsing the packet
        """
        if self.parser_state is self.ParserState.MESSAGE_CLASS:
            self.msg_class = byte
            self.parser_state = self.ParserState.ID

        elif self.parser_state is self.ParserState.ID:
            self.id = byte
            self.parser_state = self.ParserState.LENGTH_1

        elif self.parser_state is self.ParserState.LENGTH_1:
            self.length = byte
            self.parser_state = self.ParserState.LENGTH_2

        elif self.parser_state is self.ParserState.LENGTH_2:
            self.length = (byte << 8) | self.length
            if self.length > 0:
                self.parser_state = self.ParserState.PAYLOAD
            else:
                self.parser_state = self.ParserState.CHECKSUM_A

        elif self.parser_state is self.ParserState.PAYLOAD:
            self.payload.append(byte)
            self.length -= 1
            if self.length <= 0:
                self.parser_state = self.ParserState.CHECKSUM_A

        elif self.parser_state is self.ParserState.CHECKSUM_A:
            self.checksum_a = byte
            self.parser_state = self.ParserState.CHECKSUM_B

        elif self.parser_state is self.ParserState.CHECKSUM_B:
            self.checksum_b = byte
            return True
            # self.print()

            # checksum_a, checksum_b = self._generate_checksum()
            # assert(checksum_a == self.checksum_a), print("Error:", checksum_a, self.checksum_a)
            # assert(checksum_b == self.checksum_b), print("Error:", checksum_b, self.checksum_b)

        return False

    def print(self):
        """
        Print a UBX packet
        """
        self.payload = "".join([chr(byte) for byte in self.payload])
        print("[UBX] Class:{}, ID:{}, Length:{}, Payload:{}\n".format(
            self.msg_class,
            self.id,
            len(self.payload),
            self.payload
        ))

    def _generate_checksum(self):
        """
        Calculates the checksum of the current packet as a receiver
        """
        checksum_a = 0
        checksum_b = 0
        buffer = [
            self.msg_class,
            self.id,
            self.length & 0xFF,
            self.length >> 8,
        ] + self.payload

        for byte in buffer:
            checksum_a += byte
            checksum_a &= 0xFF       # Prevent 8-bit unsigned overflow
            checksum_b += checksum_a
            checksum_b &= 0xFF       # Prevent 8-bit unsigned overflow

        return checksum_a, checksum_b
