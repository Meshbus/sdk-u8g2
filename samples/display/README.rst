U8g2 rendering sample
#####################

Select ``<qualified-board-target>`` from this sample's ``sample.yaml`` and
inspect the matching configuration/overlays. Additional boards require their
own integration and validation.

This application cycles through drawing workloads and reports drawing and
flush timing. It uses ``CONFIG_U8G2`` with the board's ``zephyr,display`` chosen
device. Rendering and panel behavior require manual hardware validation.

Activate your Zephyr development environment, then build from the west
workspace root::

   west build -p always -b '<qualified-board-target>' modules/lib/u8g2/samples/display \
     -d build/sample-u8g2

Select the platform and any required shield from ``sample.yaml``. For a scenario
using a display shield, append ``-- -DSHIELD='<shield>'`` to the build command.
The sample scenarios are ``sample.display.cfb.ssd1306`` and
``sample.display.cfb.ssd16xx``. Both use U8g2 rendering.

``CONFIG_U8G2_STRESS_LEVEL`` controls workload intensity. Enable
``CONFIG_U8G2_STRESS_MODE_CYCLE`` to cycle workloads, and set
``CONFIG_U8G2_STRESS_MODE_FRAMES`` to adjust the cycle duration.
The rendering loop supports both full-buffer and
``CONFIG_U8G2_BUFFER_MODE_PAGE8`` modes.
