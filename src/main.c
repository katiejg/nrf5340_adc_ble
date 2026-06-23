/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/sys/util.h>

#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

/* STEP 3.2 - Define a variable of type adc_dt_spec for each channel */
// static const struct adc_dt_spec adc_channel = ADC_DT_SPEC_GET(DT_PATH(zephyr_user)); // single channel

static const struct adc_dt_spec adc_channels[] = {
      DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels, DT_SPEC_AND_COMMA)
};

LOG_MODULE_REGISTER(MultiChannel_ADC, LOG_LEVEL_DBG);

int main(void) {
	int err;
	uint32_t count = 0;

	/* STEP 4.1 - Define a variable of type adc_sequence and a buffer of type uint16_t */
      int16_t buf;
      struct adc_sequence sequence = {
            .buffer = &buf,
            .buffer_size = sizeof(buf),
      };

      // Configure channels individually
      for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
            /* STEP 3.3 - validate that the ADC peripheral (SAADC) is ready */
            if (!adc_is_ready_dt(&adc_channels[i])) {
                  LOG_ERR("ADC controller devivce %s not ready", adc_channels[i].dev->name);
                  return 0;
            }
            /* STEP 3.4 - Setup the ADC channel */
            err = adc_channel_setup_dt(&adc_channels[i]);
            if (err < 0) {
                  LOG_ERR("Could not setup channel #%d (%d)", 0, err);
                  return 0;
            }
      }

	while (1) {
            for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) { 
                  /* STEP 4.2 - Initialize the ADC sequence */
                  err = adc_sequence_init_dt(&adc_channels[i], &sequence);
                  if (err < 0) {
                        LOG_ERR("Could not initialize sequence");
                        return 0;
                  }

                  int32_t val_mv;

                  /* STEP 5 - Read a sample from the ADC */
                  err = adc_read(adc_channels[i].dev, &sequence);
                  if (err < 0) {
                        LOG_ERR("Could not read (%d)", err);
                        continue;
                  }

                  val_mv = (int)buf;
                  LOG_INF("ADC reading[%u]: %s, channel %d: Raw: %d", count++, adc_channels[i].dev->name,
                        adc_channels[i].channel_id, val_mv);

                  /* STEP 6 - Convert raw value to mV*/
                  err = adc_raw_to_millivolts_dt(&adc_channels[i], &val_mv);
                  if (err < 0) {
                        LOG_WRN(" (value in mV not available)\n");
                  } else {
                        LOG_INF(" = %d mV", val_mv);
                  }

                  k_sleep(K_MSEC(1000));
            }
	}
	return 0;
}