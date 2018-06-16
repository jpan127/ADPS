Import("*")
import os



# All directories with source files in the project
PROJECT_SRC_DIRS  = get_all_subdirectories(PROJECT_ROOT_DIR.Dir("main").abspath)

# All source (*.c) files in the project
PROJECT_SRC_FILES = Flatten([Dir(dir).glob("*.c") for dir in PROJECT_SRC_DIRS])

# Compile all source files into object files
PROJECT_OBJ_FILES = [
    xtensa_env.Object(
        target = "{}.o".format(os.path.splitext(src.name)[0]),
        source = src,
    )
    for src in
    PROJECT_SRC_FILES
]

# Link the existing static libraries with the compiled source files
# Creates an ELF file and a MAP file
elf_file = xtensa_env.Program(
    target = "bin/{}".format(TARGET),
    source = PROJECT_OBJ_FILES,
)

# Let SCons know there was a MAP file created as a side effect of the linker
map_file = xtensa_env.SideEffect(
    "bin/{}.map".format(TARGET),
    elf_file,
)

# Creates a binary file from the ELF file using an ESP32 script
bin_file = esp32_flasher_env.CreateBinary(
    target = "bin/{}.bin".format(TARGET),
    source = [elf_file],
)

# Specify all these builders should be built
Default([
    elf_file,
    map_file,
    bin_file
])
