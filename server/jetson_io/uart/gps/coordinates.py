import math



# http://www.movable-type.co.uk/scripts/latlong.html
# Earth's radius is a constant and is used in multiple algorithms
EARTH_RADIUS = 6371

class Coordinates:
    """
    Represents a coordinate pair { latitude, longitude }
    """

    def __init__(self, latitude_degrees, longitude_degrees):
        self.latitude_degrees  = latitude_degrees
        self.longitude_degrees = longitude_degrees
        self.latitude_radians  = math.radians(float(latitude_degrees))
        self.longitude_radians = math.radians(float(longitude_degrees))

    def __str__(self):
        return "{}, {}".format(
            self.latitude_degrees,
            self.longitude_degrees,
        )


def calculate_haversine_distance(coordinate1, coordinate2):
    """
    a = sin²(Δφ/2) + cos φ1 ⋅ cos φ2 ⋅ sin²(Δλ/2)
    c = 2 ⋅ atan2( √a, √(1−a) )
    d = R ⋅ c
    """
    delta_latitude_radians  = math.radians(coordinate2.latitude_degrees  - coordinate1.latitude_degrees)
    delta_longitude_radians = math.radians(coordinate2.longitude_degrees - coordinate1.longitude_degrees)
    a = (math.sin(delta_latitude_radians / 2) ** 2)     + \
            math.cos(coordinate1.latitude_radians)      * \
            math.cos(coordinate2.latitude_radians)      * \
            (math.sin(delta_longitude_radians / 2) ** 2)
    c = 2 * math.atan2(math.sqrt(a), math.sqrt(1 - a))
    d = EARTH_RADIUS * c
    return d


def calculate_initial_bearing(coordinate1, coordinate2):
    """
    θ = atan2( sin Δλ ⋅ cos φ2 , cos φ1 ⋅ sin φ2 − sin φ1 ⋅ cos φ2 ⋅ cos Δλ )
    https://www.igismap.com/map-tool/bearing-angle
    """
    delta_longitude_radians = coordinate2.longitude_radians - coordinate1.longitude_radians
    y = math.sin(delta_longitude_radians) * math.cos(coordinate2.latitude_radians)
    x = math.cos(coordinate1.latitude_radians) * math.sin(coordinate2.latitude_radians) - \
        math.sin(coordinate1.latitude_radians) * math.cos(coordinate2.latitude_radians) * math.cos(delta_longitude_radians)
    initial_bearing = math.degrees(math.atan2(y, x))
    return initial_bearing


def determine_closest_direction_from_bearing(bearing_degrees) -> str:
    DEGREE_DIRECTION_MAP = {
        0   : "N",
        45  : "NE",
        90  : "E",
        135 : "SE",
        180 : "S",
        225 : "SW",
        270 : "W",
        315 : "NW",
    }

    # Clamp to under 360
    bearing_degrees %= 360
    # Find the lower bound of multiple of 45
    multiples = bearing_degrees // 45
    # Find remainder of 45
    bearing_degrees %= 45
    # Determine if closer to lower bound or upper bound
    rounded_degree = (multiples + 1) if bearing_degrees >= (45 / 2) else multiples
    print(multiples, bearing_degrees, rounded_degree)
    return DEGREE_DIRECTION_MAP[rounded_degree * 45]


def calculate_projected_coordinate(coordinate1, initial_bearing, distance) -> Coordinates:
    """
    φ2 = asin( sin φ1 ⋅ cos δ + cos φ1 ⋅ sin δ ⋅ cos θ )
    λ2 = λ1 + atan2( sin θ ⋅ sin δ ⋅ cos φ1, cos δ − sin φ1 ⋅ sin φ2 )
    """
    distance_angle = distance / EARTH_RADIUS
    latitude = math.asin(math.sin(coordinate1.latitude) * math.cos(distance_angle) +
                         math.cos(coordinate1.latitude) * math.sin(distance_angle) * math.cos(initial_bearing))
    y = math.sin(initial_bearing) * math.sin(distance_angle) * math.cos(coordinate1.latitude)
    x = math.cos(distance_angle) - math.sin(coordinate1.latitude) * math.sin(latitude)
    longitude = coordinate1.longitude + math.atan2(y, x)

    return Coordinates(latitude, longitude)


def _main():
    print("Quick example test..")
    coordinate1 = Coordinates(37.234566, -121.846145)
    coordinate2 = Coordinates(37.237569, -121.845640)
    print("{:.5f} km             ".format(calculate_haversine_distance(coordinate1, coordinate2)))
    print("{:.5f} initial bearing".format(calculate_initial_bearing(coordinate1, coordinate2)))
    print(determine_closest_direction_from_bearing(calculate_initial_bearing(coordinate1, coordinate2)))

if __name__ == '__main__':
    _main()
