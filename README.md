## nRF5340-DK ADC Readings
Based off [this lesson](https://github.com/NordicDeveloperAcademy/ncs-inter/tree/main/l6/l6_e1) from Nordic Academy.

Reads ADC channels 0, 1, 2.

Comment out `adc_original.c` when flashing `main.c`.

- `adc_original.c`: Constantly outputs values from ADC channels.
- `main.c`: Uses LED and Button GPIO to indicate and control reading outputs. 

## Build Configuration
Defaults + 
- Board target = `nrf5340dk/nrf5340/cpuapp`
- Base configuration files = `prj.conf`
- Base Devicetree overlays = `boards/nrf5340dk_nrf5340_cpuapp.overlay`