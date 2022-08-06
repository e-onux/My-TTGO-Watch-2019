#pragma once


#define TWATCH_TFT_MISO             (gpio_num_t)0
#define TWATCH_TFT_MOSI             (gpio_num_t)19
#define TWATCH_TFT_SCLK             (gpio_num_t)18
#define TWATCH_TFT_CS               (gpio_num_t)5
#define TWATCH_TFT_DC               (gpio_num_t)27
#define TWATCH_TFT_RST              (gpio_num_t)-1

#define TOUCH_SDA                   23
#define TOUCH_SCL                   32
#define TOUCH_INT                   38

#define SEN_SDA                     21
#define SEN_SCL                     22

#define RTC_INT_PIN                     37
#define AXP202_INT                  35
#define BMA423_INT1                 39

#define MOTOR_PIN                   33



/**
 * BMA423 interrupts
 */

    #define BMA_INT_1   39
    #define BMA_INT_2   0
    /**
     * Button pins
     */
    #define BTN_1       36
    //#define BTN_2       34
    //#define BTN_3       35
    /**
     * I2C pins
     */
    #define IICSCL      25
    #define IICSDA      26
 

