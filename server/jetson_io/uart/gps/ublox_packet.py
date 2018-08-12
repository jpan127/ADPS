from abc import abstractmethod, ABCMeta



class UbloxPacket(metaclass=ABCMeta):
    """
    Abstract base class for a received packet from a Ublox GPS
    """

    @abstractmethod
    def parse(self, byte):
        """
        Parses a packet and determines where the current byte fits in the packet
        """
        raise NotImplementedError("Child class did not define this function")

    @abstractmethod
    def print(self):
        """
        Prints the packet when completely parsed
        """
        raise NotImplementedError("Child class did not define this function")
