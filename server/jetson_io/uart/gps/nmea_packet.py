from enum import Enum, unique
from ublox_packet import UbloxPacket
from nmea_gpgll_packet import NmeaGpgllPacket
import nominatim
from coordinates import Coordinates



class NmeaPacket(UbloxPacket):
    """
    Packet that follows the NMEA protocol frame format
    :documentation : Page 61
    """

    @unique
    class ParserState(Enum):
        """
        Enumerates the current state of the parser
        """
        TALKER_ID          = 0
        SENTENCE_FORMATTER = 1
        PAYLOAD            = 2
        CHECKSUM           = 3
        CARRIAGE_RETURN    = 4
        LINE_FEED          = 5

    def __init__(self):
        """
        Initializer
        """
        self.talker_id          = ""
        self.sentence_formatter = ""
        self.payload            = []
        self.checksum_a         = None
        self.checksum_b         = None
        self.parser_state       = self.ParserState.TALKER_ID
        self.gpgll_packet       = None

    def parse(self, byte):
        """
        Determines where the current byte fits into the packet
        :returns : True  if packet is done parsing
                   False if packet is not done yet
                   None  if there was an error parsing the packet
        """
        # pylint: disable=too-many-branches
        if self.parser_state is self.ParserState.TALKER_ID:
            self.talker_id += chr(byte)
            # Always 2 characters
            if len(self.talker_id) >= 2:
                self.parser_state = self.ParserState.SENTENCE_FORMATTER

        elif self.parser_state is self.ParserState.SENTENCE_FORMATTER:
            self.sentence_formatter += chr(byte)
            # Always 3 characters
            if len(self.sentence_formatter) >= 3:
                self.parser_state = self.ParserState.PAYLOAD

        elif self.parser_state is self.ParserState.PAYLOAD:
            # '*' is right between payload and checksum
            if byte == ord('*'):
                self.parser_state = self.ParserState.CHECKSUM
            else:
                self.payload.append(byte)

        elif self.parser_state is self.ParserState.CHECKSUM:
            # Convert them into characters, then append as a string, then convert to hex
            if not self.checksum_a:
                self.checksum_a = byte
            elif not self.checksum_b:
                self.checksum_b = byte
                self.parser_state = self.ParserState.CARRIAGE_RETURN

        elif self.parser_state is self.ParserState.CARRIAGE_RETURN:
            if byte == ord('\r'):
                self.parser_state = self.ParserState.LINE_FEED
            else:
                print("[ERROR] Missing '\\r', got {}".format(byte))
                self.print()
                return None

        elif self.parser_state is self.ParserState.LINE_FEED:
            if byte == ord('\n'):
                self.print()
                return True
            else:
                print("[ERROR] Missing '\\n', got {}".format(byte))
                self.print()
                return None

        return False

    def get_coordinates(self):
        """
        Extract coordinates from a GPGLL payload
        Converts NMEA coordinates into decimal coordinates
        """
        DIRECTION_TO_SIGN_MAP = {
            'N' : +1,
            'S' : -1,
            'E' : +1,
            'W' : -1,
        }

        # Split string after the second character because the format in NMEA is DDMM.MMMM
        degrees, minutes = float(self.gpgll_packet.latitude[:2]), float(self.gpgll_packet.latitude[2:])
        self.gpgll_packet.latitude = degrees + (minutes / 60)

        # Split string after the third character because the format in NMEA is DDDMM.MMMM
        degrees, minutes = float(self.gpgll_packet.longitude[:3]), float(self.gpgll_packet.longitude[3:])
        self.gpgll_packet.longitude = degrees + (minutes / 60)

        # Get direction and use that to determine the +/- sign of the coordinates
        try:
            latitude_sign = DIRECTION_TO_SIGN_MAP[self.gpgll_packet.vertical_direction]
        except KeyError:
            print("Invalid direction found : {}, expected 'S' or 'N'".format(self.gpgll_packet.vertical_direction))
            return 0, 0
        try:
            longitude_sign = DIRECTION_TO_SIGN_MAP[self.gpgll_packet.horizontal_direction]
        except KeyError:
            print("Invalid direction found : {}, expected 'S' or 'N'".format(self.gpgll_packet.horizontal_direction))
            return 0, 0

        self.gpgll_packet.latitude  *= latitude_sign
        self.gpgll_packet.longitude *= longitude_sign

        return self.gpgll_packet.latitude, self.gpgll_packet.longitude

    def print(self):
        """
        Print a NMEA packet
        """
        message_key = self.talker_id + self.sentence_formatter

        if message_key == "GPGLL":
            self.gpgll_packet = NmeaGpgllPacket(self.payload)
            if self.gpgll_packet.valid:
                latitude, longitude = self.get_coordinates()

                print("[GPGLL] Latitude: {:.15f} | Longitude: {:.15f}".format(
                    latitude,
                    longitude
                ))

                nominatim.reverse_geocode(Coordinates(latitude, longitude))
