import csv
import random

citys = {
    "Detroit": (26.4, 6.07),
    "Pontiac": (23.1, 3.8),
    "Ann Arbor": (27.0, 8.28),
    "Grand Rapids": (11.4, 4.5),
    "Muskegon": (13.0, 4.57),
    "Lansing": (23.18, 8.4),
    "Flint": (25.5, 4.76),
    "Saginaw": (27.43, 6.9),
    "Traverse City": (10.8, 4.25),
    "Alpena": (20.48, 3.2),
    "Sault Ste. Marie": (24.8, 7.73),
    "Ironwood": (20.83, 4.2),
    "Iron Mountain": (25.8, 8.93),
    "Escanaba": (12.73, 3.4),
    "Marquette": (17.8, 6.2),
    "Houghton": (22.0, 6.8),
    "Chicago": (13.6, 6.54),
    "Los Angeles": (15.14, 3.2),
    "San Francisco": (19.5, 3.82),
    "New York": (25.86, 3.9),
    "Buffalo": (16.3, 7.48),
    "Boston": (17.96, 7.1),
    "Washington": (15.5, 6.21),
    "Dulles": (15.38, 6.8),
    "Atlanta": (27.1, 3.17),
    "Miami": (15.34, 3.3),
    "New Orleans": (24.9, 7.34),
    "Dallas": (11.87, 5.8),
    "Houston": (11.8, 6.23),
    "Denver": (11.75, 8.6),
    "Phoenix": (24.7, 4.02),
    "Seattle": (14.62, 4.2),
    "Portland": (18.6, 6.27),
    "Salt Lake City": (10.51, 5.2),
    "Las Vegas": (19.5, 4.93),
    "Honolulu": (10.81, 4.9),
    "Anchorage": (16.8, 4.3),
    "Vancouver": (22.36, 7.2),
    "Toronto": (22.3, 7.05),
    "Montreal": (17.55, 6.6),
    "London": (14.5, 3.01),
    "Tokyo": (23.25, 5.6),
    "Sydney": (23.8, 3.27)
}

cities = list(citys.keys())
filename = "measurements.csv"
num_rows = 1000000000

with open(filename, mode='w') as file:
    writer = csv.writer(file)

    for _ in range(num_rows):
        city = random.choice(cities)
        mean, std_dev = citys[city]

        value = round(random.gauss(mean, std_dev), 1)

        writer.writerow([city, value])

print("done")