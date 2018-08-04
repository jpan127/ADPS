import pprint
import json
import requests

from address import Address
from coordinates import Coordinates



# Sample values:
# [GPGLL] Latitude: 37.234596000000003 | Longitude: -121.846007166666666
# [GPGLL] Latitude: 37.234689000000003 | Longitude: -121.845889000000000
# [GPGLL] Latitude: 37.234673000000001 | Longitude: -121.845871000000002

pp = pprint.PrettyPrinter(indent=4)

def reverse_geocode(coordinates: Coordinates) -> Address:
    """
    Performs a reverse geocode query given a coordinate pair and returns an address
    :param coordinates : Set of coordinates to convert to an address
    :param returns     : Dictionary of values if successful, otherwise None
    """
    request = requests.get(
        "http://localhost/nominatim/reverse.php?format=json&lat={}&lon={}".format(
            coordinates.latitude_degrees,
            coordinates.longitude_degrees
        )
    )

    if request.status_code == 200:
        request_json  = json.loads(request.text)
        minified_json = {
            "house_number" : request_json["address"]["house_number"],
            "road"         : request_json["address"]["road"],
            "city"         : request_json["address"]["city"],
            "state"        : request_json["address"]["state"],
            "postcode"     : request_json["address"]["postcode"],
        }

        # pp.pprint(minified_json)
        return Address(**minified_json)
    else:
        print("Error sending request :", request.status_code)
        return None


def forward_geocode(address: Address) -> Coordinates:
    """
    Performs a forward geocode query given a full address and returns a coordinate pair
    :param adddress : Address object that must have at least house number, street name, and city
    :param returns  : Coordinate object with the address' coordinates
    """
    request_url = "&".join([
        "http://localhost/nominatim/search.php?format=json",
        "q={}".format(str(address)),
        "polygon_geojson=1",
        "viewbox=",
    ])

    request = requests.get(request_url)

    if request.status_code == 200:
        request_json  = json.loads(request.text)
        minified_json = {
            "bounding_box" : request_json[0]["boundingbox"],
            "latitude"     : request_json[0]["lat"],
            "longitude"    : request_json[0]["lon"],
        }

        return Coordinates(
            latitude_degrees  = minified_json["latitude"],
            longitude_degrees = minified_json["longitude"],
        )
    else:
        print("Error sending request :", request.status_code)
        return None


def _main():
    """
    Test program
    @TODO : Move this to a unit test
    """
    test_addresses = [
        Address(
            house_number = "6166",
            road         = "Chesbro Avenue",
            city         = "San José",
            state        = "California",
            postcode     = "95123",
        ),
        Address(
            house_number = "790",
            road         = "Pilgrim Loop",
            city         = "Fremont",
            state        = "California",
            postcode     = "94539",
        ),
    ]

    for i, address in enumerate(test_addresses):
        print("Test", i)
        coordinates = forward_geocode(address)
        end_address = reverse_geocode(coordinates)
        print("\tAddress     :", address    )
        print("\tCoordinates :", coordinates)
        print("\tAddress     :", end_address)
        assert address == end_address

if __name__ == '__main__':
    _main()
