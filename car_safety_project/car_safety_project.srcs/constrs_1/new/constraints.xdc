set_property PACKAGE_PIN N8 [get_ports {GPIO_O_0 }]; #Arduino D7 pin
set_property IOSTANDARD LVCMOS33 [get_ports GPIO_O_0]

# ----------------------------------------------------------------------------
# I2C bus 
# ---------------------------------------------------------------------------- 
# Bank 35
set_property PACKAGE_PIN G15 [get_ports {IIC_0_0_scl_io        }];  # "G15.I2C_SCL"
set_property PACKAGE_PIN F15 [get_ports {IIC_0_0_sda_io         }];  # "F15.I2C_SDA"
# Set the bank voltage for IO Bank 35 to 3.3V
set_property IOSTANDARD LVCMOS33 [get_ports -of_objects [get_iobanks 35]];