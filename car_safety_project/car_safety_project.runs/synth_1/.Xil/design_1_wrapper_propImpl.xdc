set_property SRC_FILE_INFO {cfile:/home/taifur/car_safety_project/car_safety_project.srcs/constrs_1/new/constraints.xdc rfile:../../../car_safety_project.srcs/constrs_1/new/constraints.xdc id:1} [current_design]
set_property src_info {type:XDC file:1 line:1 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN N8 [get_ports {GPIO_O_0 }]; #Arduino D7 pin
set_property src_info {type:XDC file:1 line:8 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN G15 [get_ports {IIC_0_0_scl_io        }];  # "G15.I2C_SCL"
set_property src_info {type:XDC file:1 line:9 export:INPUT save:INPUT read:READ} [current_design]
set_property PACKAGE_PIN F15 [get_ports {IIC_0_0_sda_io         }];  # "F15.I2C_SDA"
set_property src_info {type:XDC file:1 line:11 export:INPUT save:INPUT read:READ} [current_design]
set_property IOSTANDARD LVCMOS33 [get_ports -of_objects [get_iobanks 35]];
