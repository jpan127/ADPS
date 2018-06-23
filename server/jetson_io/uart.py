from enum import Enum



class _PacketDecodeState(Enum):
    """
    Enumerates the states of the packet decoding state machine
    """
    TYPE    = 0
    LENGTH  = 1
    PAYLOAD = 2


class LogPacket(object):
    """
    Object / structure for a logging packet
    """

    def __init__(self, **kwargs):
        """
        Initializer
        """
        self.state   = _PacketDecodeState.TYPE
        self.type    = kwargs.get("type"    ,  0)
        self.length  = kwargs.get("length"  ,  0)
        self.payload = kwargs.get("payload" , "")


async def _decode_state_machine(packet, byte):
    """
    Decodes a packet byte by byte
    @param packet : LogPacket object
    @param byte   : The current byte to be populated into the packet
    """
    done = False

    if _PacketDecodeState.TYPE is packet.state:
        packet.type  = byte
        packet.state = _PacketDecodeState.LENGTH
    elif _PacketDecodeState.LENGTH is packet.state:
        packet.length = byte
        packet.state  = _PacketDecodeState.PAYLOAD
    elif _PacketDecodeState.PAYLOAD is packet.state:
        packet.payload += str(byte)
        packet.length -= 1
        if packet.length <= 0:
            done = True

    return done


async def process_uart(packet_queue, uart, current_packet):
    """
    Event loop callback for when the UART port detects data
    Processes data from the UART port and enqueues into the given queue
    @param uart           : UART serial object
    @param current_packet : Current packet being decoded, modified byte by byte
    """
    # Read data
    bytes_read = uart.read(1000)
    # For each byte read, decode
    for byte in range(len(bytes_read)):
        # If the state machine says decoding is done, enqueue packet
        if await _decode_state_machine(current_packet, byte):
            await packet_queue.put(current_packet)
            # Reset packet
            current_packet = LogPacket()
