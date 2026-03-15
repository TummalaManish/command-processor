from conan import ConanFile
from conan.tools.cmake import cmake_layout

class CmdProcRecipe(ConanFile):
    name = "cmd-proc"
    version = "0.1.0"

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}

    def layout(self):
        cmake_layout(self)

    def requirements(self):
        # Production dependencies go here
        pass

    def build_requirements(self):
        # Tools go here (GTest is now handled by FetchContent in CMake)
        self.tool_requires("cmake/[>=3.25]")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def generate(self):
        pass
