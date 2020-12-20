import json, os
from conans import ConanFile, CMake, tools


class ONVIFMonitorConan(ConanFile):
    jsonInfo = json.loads(tools.load("info.json"))
    name = jsonInfo["projectName"]
    version = "%u.%u.%u" % (jsonInfo["version"]["major"], jsonInfo["version"]["minor"], jsonInfo["version"]["patch"])
    license = jsonInfo["license"]
    url = jsonInfo["repository"]
    description = jsonInfo["projectDescription"]
    author = jsonInfo["vendor"]
    homepage = jsonInfo["repository"]
    requires = "Qt/[>=5.10 <6.0]@tereius/stable", "libONVIF/1.1.0-SNAPSHOT@tereius/stable", "QtAV/1.13.0-SNAPSHOT@tereius/stable"
    settings = "os", "compiler", "build_type", "arch"
    default_options = "Qt:shared=True", "Qt:openssl=True", "Qt:qtbase=True", "Qt:qtsvg=True", "Qt:qtdeclarative=True", "Qt:qttools=True", "Qt:qttranslations=True", "Qt:qtrepotools=True", "Qt:qtqa=True", "Qt:qtgraphicaleffects=True", "Qt:qtquickcontrols=True", "Qt:qtquickcontrols2=True"
    generators = "cmake"
    exports = "info.json"
    exports_sources = "*"

    def build(self):
        tools.replace_in_file(os.path.join(self.build_folder, "CMakeLists.txt"), "### CONAN_BEACON ###", 'include(%s)\n%s' % (os.path.join(self.install_folder, "conanbuildinfo.cmake").replace("\\", "/"), "conan_basic_setup()"), strict=False)
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.install()
