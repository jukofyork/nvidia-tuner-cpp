#pragma once

constexpr int MAJOR_MIN_VERSION = 520;

constexpr unsigned int MAX_FAN_SPEED = 100;                  // %
constexpr unsigned int MAX_TEMPERATURE = 100;                // °C

constexpr float DEFAULT_PROPORTIONAL_GAIN = 1.5f;
constexpr float MIN_PROPORTIONAL_GAIN = 0.1f;
constexpr float MAX_PROPORTIONAL_GAIN = 10.0f;

constexpr float DEFAULT_INTEGRAL_GAIN = 0.05f;
constexpr float MIN_INTEGRAL_GAIN = 0.01f;
constexpr float MAX_INTEGRAL_GAIN = 1.0f;

constexpr unsigned int DEFAULT_FAN_SPEED_UPDATE_PERIOD = 2;  // s
constexpr unsigned int MIN_FAN_SPEED_UPDATE_PERIOD = 1;      // s
constexpr unsigned int MAX_FAN_SPEED_UPDATE_PERIOD = 10;     // s

constexpr float MIN_TARGET_TEMPERATURE = 30.0f;             // °C
constexpr float MAX_TARGET_TEMPERATURE = 90.0f;             // °C

constexpr float BASELINE_FAN_SPEED = 30.0f;                 // %
