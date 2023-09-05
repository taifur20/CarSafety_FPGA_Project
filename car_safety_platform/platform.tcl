# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/taifur/workspace/car_safety_platform/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/taifur/workspace/car_safety_platform/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {car_safety_platform}\
-hw {/home/taifur/car_safety_project/design_1_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -out {/home/taifur/workspace}

platform write
platform generate -domains 
platform active {car_safety_platform}
platform generate
