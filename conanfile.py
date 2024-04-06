 from conan.base import ConanFile
from conan.tools import settings

class MyCPPProject(ConanFile):
    name = "my_cpp_project"  # Replace with your project name
    version = "0.1.0"  # Replace with your desired version

    # Define a profile named "my_profile" (replace with your desired profile name)
    settings.define("build_type", default="Release", descriptions="Build type")
    settings.options.add("boost", "shared", default=False, descriptions="Boost library linkage")

    requires = [
        "boost/1.76.0@conan_boost/channels",  # Example dependency
    ]

    def build(self):
        # Replace with your actual build commands
        cmake = tools.get_tool("cmake")
        cmake.configure(build_dir=self.build_folder)
        cmake.generate(build_dir=self.build_folder)
        cmake.build(build_dir=self.build_folder)

    def package(self):
        self.copy("*.h", dst="include", src=self.source_folder)  # Copy headers
        self.copy("*.lib", dst="lib", src=self.build_folder)  # Copy libraries (adjust for .so or .dll)
        self.copy("*.a", dst="lib", src=self.build_folder)  # Copy archives (adjust for .so or .dll)
        # Copy other relevant files (executables, data) as needed
