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
    requires = "Qt/[^5.14]@tereius/stable", "libONVIF/2.0.0-SNAPSHOT@tereius/stable", "QtAV/1.13.0-SNAPSHOT@tereius/stable", "Kirigami/5.80.0@tereius/stable"
    settings = "os", "compiler", "build_type", "arch"
    options = {"installApk": [True, False]}
    default_options = "Qt:shared=True", "Qt:openssl=True", "Qt:qtbase=True", "Qt:qtsvg=True", "Qt:qtdeclarative=True", "Qt:qttools=True", "Qt:qttranslations=True", "Qt:qtrepotools=True", "Qt:qtqa=True", "Qt:qtgraphicaleffects=True", "Qt:qtquickcontrols=True", "Qt:qtquickcontrols2=True", "installApk=False"
    generators = "cmake"
    exports = "info.json"
    exports_sources = "*"

    def build_requirements(self):
        self.build_requires("QtDeployHelper/1.0.0@tereius/stable", force_host_context=True)

    def build(self):
        tools.replace_in_file(os.path.join(self.build_folder, "CMakeLists.txt"), "### CONAN_BEACON ###", 'include(%s)\n%s' % (os.path.join(self.install_folder, "conanbuildinfo.cmake").replace("\\", "/"), "conan_basic_setup()"), strict=False)
        cmake = CMake(self)
        #cmake.definitions["CMAKE_FIND_DEBUG_MODE"] = "ON"
        cmake.configure()
        cmake.build()
        if self.settings.os == 'Android':
            cmake.build(target='make-apk')
            if self.options.installApk:
                cmake.build(target='apk-install')
        else:
            cmake.install()

    def package(self):
        if self.settings.os == 'Android':
            self.copy("*.apk")

    def package_id(self):
        del self.info.options.installApk
