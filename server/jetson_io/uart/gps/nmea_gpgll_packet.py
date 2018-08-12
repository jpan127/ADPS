
class ParserException(Exception):
    pass

class NmeaGpgllPacket:
    """
    NMEA GPGLL packet definition
    """

    NUM_FIELDS = 7

    def __init__(self, payload):
        self.latitude             = None
        self.vertical_direction   = None
        self.longitude            = None
        self.horizontal_direction = None
        self.timestamp            = None
        self.valid                = None
        self.mode_and_checksum    = None
        self._convert_payload(payload)

    def _convert_payload(self, payload):
        def __convert_payload(latitude, vertical_direction, longitude, horizontal_direction, timestamp, valid, mode_and_checksum):
            self.latitude             = latitude
            self.vertical_direction   = vertical_direction
            self.longitude            = longitude
            self.horizontal_direction = horizontal_direction
            self.timestamp            = timestamp
            self.valid                = (valid == "A")
            self.mode_and_checksum    = mode_and_checksum
        payload = "".join([chr(byte) for byte in payload[1:]])
        payload = payload.split(",")
        if len(payload) != self.NUM_FIELDS:
            raise ParserException("[NMEA][GPGLL] Packet payload incorrect : " + payload)
        else:
            __convert_payload(*payload)
