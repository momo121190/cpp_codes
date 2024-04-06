from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class Pkg(ConanFile):
   name = "mypkg"
   # see, no version defined!
   def requirements(self):
       # this trick allow to depend on packages on your same user/channel
       self.requires("dep/0.3@%s/%s" % (self.user, self.channel))

   def build(self):
       if self.version == "myversion":
           # something specific for this version of the package.