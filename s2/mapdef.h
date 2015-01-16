
#pragma once


#define OMAPX   180
#define OMAPY   180

#define OVERMAP_DEPTH   10
#define OVERMAP_HEIGHT  0
#define OVERMAP_LAYERS  (1 + OVERMAP_DEPTH + OVERMAP_HEIGHT)

#define OVERMAP_CITY_SIZE()         dice(3, 6)
#define OVERMAP_CITIES_PER_BLOCK()  dice(3, 4)
#define OVERMAP_ROAD_LENGTH()       rng(1, 3)
#define OVERMAP_STREET_CHANCE       5
#define OVERMAP_SWAMPINESS          4
#define OVERMAP_NUM_FOREST          250
#define OVERMAP_SWAMPCHANCE         8500 // Chance that a swamp will spawn instead of forest
#define OVERMAP_TOP_HIWAY_DIST      999


#define SMAPX   24
#define SMAPY   24



