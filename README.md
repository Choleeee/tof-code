# tof-code for F405RGT6 

Refer to [2025 Advanced Tutorials](https://github.com/UST-Robotics-Team/Software-Tutorial-2025-Notes/blob/main/advanced-tutorial-1-Advanced-Embedding-System/5-I2C.md)
### Steps:
1. Generate Code from ioc
2. drag the `vl53l1_API` folder into your `Drivers` folder
3. Build the project
4. flash code

### Code:
In `main.c`

Create definition
```
/* USER CODE BEGIN PV */
tof_vl53l1_dev_t dev1;
/* USER CODE END PV */
```

Verify and init the sensor
```
/* USER CODE BEGIN 2*/

// Code for verifying i2c address of the tof sensor
...

dev1.I2cDevAddr = sensor_address;
int sensor_status = VL53L1X_SensorInit(dev1.I2cDevAddr); //inits tof at the correct address

...
// Code for testing sensor's functions
```

Getting the sample result from dev1 (inside while loop)
```
while (1) {
    int sample_status = tof_regular_sample(&dev1); 

    if (sample_status == 0) {
        uint16_t distance_mm = dev1.Distance;
        uint8_t range_status = dev1.RangeStatus; // 0 = valid measurement
        tft_prints(0, 6, "D: %d mm", distance_mm);
        tft_prints(0, 7, "RS: %d", range_status);
    } else {
      tft_prints(0, 6, "Sample err: %d", sample_status);
      tft_prints(0, 7, "No valid range");
      dev1.Distance = 0;
    }
...
```
