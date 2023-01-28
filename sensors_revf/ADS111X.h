#pragma once
#ifndef ADS111X_H
#define ADS111X_H

typedef enum : uint8_t {
        TWO_THIRDS,
        ONE,
        TWO,
        FOUR,
        EIGHT,
        SIXTEEN
} gainValue;

typedef enum : uint8_t {
        SPEED_EIGHT,
        SPEED_SIXTEEN,
        THIRTY_TWO,
        SIXTY_FOUR,
        ONE_HUNDRED_TWENTY_EIGHT,
        TWO_HUNDRED_FIFTY,
        FOUR_HUNDRED_SEVENTY_FIVE,
        EIGHT_HUNDRED_SIXTY
} speedValue;

typedef enum : uint8_t {
        A0_A1,
        A0_A3,
        A1_A3,
        A2_A3,
        A0,
        A1,
        A2,
        A3,
} multiplexer;

#endif
