from conans import ConanFile, tools, CMake

class Conan (ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = ("fmt/7.1.3", "spdlog/1.8.2", "doctest/2.4.4", "rapidcheck/20210107")
    generators = "cmake_paths", "cmake_find_package", "cmake_find_package_multi"

    def imports(self):
        self.copy("*.dll", dst="bin", src="bin")        # From bin to bin
        self.copy("*.dylib*", dst="bin", src="lib")     # From lib to bin

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
