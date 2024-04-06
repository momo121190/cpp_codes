from conan.base import ConanFile
from conan.tools import settings, tools, os

class MyCPPProject(ConanFile):
    name = "my_cpp_project"  # Replace with your project name
    version = "0.1.0"  # Replace with your desired version

    generators = ["cmake"]

    def build(self):
        cmake = tools.get_tool("cmake")
        cmake.configure(build_dir=self.build_folder)
        cmake.generate(build_dir=self.build_folder)
        cmake.build(build_dir=self.build_folder)

    def package(self):
        self.copy("*.h", dst="include", src=self.source_folder)  # Copy headers
        self.copy("*.lib", dst="lib", src=self.build_folder)  # Copy libraries (adjust for .so or .dll)
        self.copy("*.a", dst="lib", src=self.build_folder)  # Copy archives (adjust for .so or .dll)
        # Copy other relevant files (executables, data) as needed

    def package_info(self):
        self.cpp_info.libs = ["my_cpp_project"]  # Replace with your library name
        self.cpp_info.includedirs = ["include"]  # Add header directories as needed
