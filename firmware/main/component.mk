#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the 
# src/ directory, compile them and link them into lib(subdirectory_name).a 
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

COMPONENT_SRCDIRS 			:= 	L0_drivers/src 	\
								L1_io/src 		\
								L1_utils/src	\
								L2_app/src		\
								L3_tasks/tasks	\
								main

COMPONENT_ADD_INCLUDEDIRS 	:= 	L0_drivers 		\
								L1_io 			\
								L1_utils		\
								L2_app			\
								L3_tasks		\
								main