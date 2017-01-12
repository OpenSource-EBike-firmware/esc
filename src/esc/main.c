/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <esc/timing.h>
#include <esc/init.h>
#include <esc/helpers.h>
#include <esc/serial.h>
#include <esc/param.h>
#include <esc/adc.h>
#include <esc/pwm.h>
#include <esc/drv.h>
#include <esc/motor.h>
#include <esc/encoder.h>
#include <esc/can.h>
#include <esc/program.h>
#include <esc/semihost_debug.h>
#include <stdio.h>

int main(void)
{
    uint8_t prev_seq = 0;

    clock_init();
    timing_init();
    serial_init();
    canbus_init();
    param_init();
    spi_init();
    drv_init();
    adc_init();
    pwm_init();
    usleep(100000);
    motor_init();

    program_init();

    uint32_t last_print_ms = 0;

    // main loop
    while(1) {
        // wait specified time for adc measurement
        struct adc_sample_s adc_sample;
        uint8_t d_seq;
        do {
            encoder_read_angle();
            adc_get_sample(&adc_sample);
            d_seq = adc_sample.seq-prev_seq;
        } while (d_seq < 3);
        prev_seq = adc_sample.seq;
        float dt = d_seq*adc_get_smp_period();

        program_event_adc_sample(dt, &adc_sample);

        uint32_t tnow_ms = millis();
        if (tnow_ms-last_print_ms >= 2000) {
//             semihost_debug_printf("%d mV\n", (int32_t)(motor_get_vbatt()*1000));
            drv_print_faults();
            last_print_ms = tnow_ms;
//             drv_write_register_bits(0x9,1,1,0b1); // clear faults
        }
    }

    return 0;
}
