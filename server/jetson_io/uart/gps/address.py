
class Address:
    """
    Structure that holds all the information for an address
    """

    def __init__(self, **kwargs):
        """
        Initializer
        """
        self.house_number = kwargs.get("house_number", "None")
        self.road         = kwargs.get("road"        , "None")
        self.city         = kwargs.get("city"        , "None")
        self.state        = kwargs.get("state"       , "None")
        self.postcode     = kwargs.get("postcode"    , "None")

    def dict(self):
        """
        Packs this instance's object as a dictionary and returns it
        """
        return {
            "house_number" : self.house_number,
            "road"         : self.road,
            "city"         : self.city,
            "state"        : self.state,
            "postcode"     : self.postcode,
        }

    def __str__(self):
        """
        Packs this instance's object as a '+' separated string and returns it
        """
        space_separated_str = " ".join([
            self.house_number,
            self.road,
            self.city,
            self.state,
            self.postcode,
        ])

        return space_separated_str.replace(" ", "+")

    def __eq__(self, other):
        """
        Overloaded comparison
        """
        return self.__dict__ == other.__dict__
