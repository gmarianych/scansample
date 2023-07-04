from conans import ConanFile, CMake
import os
import shutil

class Recipie(ConanFile):
    requires = "gtest/1.10.0", "cryptopp/8.7.0"#"boost/1.78.0"
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake", "cmake_paths"

    def config_options(self):
        if self.settings.os == "Windows":
            pass
        elif self.settings.os == "Linux":
            pass
        elif self.settings.os == "Macos":
            pass

    def configure(self):
        os.environ['CONAN_REVISIONS_ENABLED'] = '1'
        if self.settings.os == "Windows":
            pass
        elif self.settings.os == "Linux":
            pass
        elif self.settings.os == "Macos":
            pass

    def requirements(self):
        print('requirements')

    def imports(self):
        print('[Imports]: ' + str(self.settings.os))
