#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json, os
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.files import copy
from conan.tools.build import cross_building
from conan.tools.env import VirtualBuildEnv

required_conan_version = ">=2.0"


class ONVIFMonitorConan(ConanFile):
    jsonInfo = json.load(open("info.json", 'r'))
    # ---Package reference---
    name = jsonInfo["projectName"].lower()
    version = "%u.%u.%u" % (jsonInfo["version"]["major"], jsonInfo["version"]["minor"], jsonInfo["version"]["patch"])
    user = jsonInfo["domain"]
    channel = "%s" % ("snapshot" if jsonInfo["version"]["snapshot"] else "stable")
    # ---Metadata---
    description = jsonInfo["projectDescription"]
    license = jsonInfo["license"]
    author = jsonInfo["vendor"]
    topics = jsonInfo["topics"]
    homepage = jsonInfo["homepage"]
    url = jsonInfo["repository"]
    # ---Requirements---
    requires = ["qt/6.8.2@de.privatehive/stable",
                "libonvif/3.0.1@de.privatehive/stable",
                "materialrally/[~1]@de.privatehive/snapshot",
                "qtappbase/1.5.0@de.privatehive/snapshot",
                "mdk-sdk/0.30.1@de.privatehive/stable",
                "openssl/3.0.15@de.privatehive/stable"
                ]
    tool_requires = ["cmake/[>=3.21.7]", "ninja/[>=1.11.1]"]
    # ---Sources---
    exports = ["info.json", "LICENSE"]
    exports_sources = ["info.json", "*.txt", "src/*", "resources/*", "CMake/*"]
    # ---Binary model---
    settings = "os", "compiler", "build_type", "arch"
    options = {}
    default_options = {
        "libonvif/*:openssl": True,
        "qtappbase/*:qml": True,
        "qtappbase/*:secretsManager": True,
        "qt/*:GUI": True,
        "qt/*:opengl": "desktop",
        "qt/*:openssl": True,
        "qt/*:qtbase": True,
        "qt/*:widgets": True,
        "qt/*:qtdeclarative": True,
        "qt/*:qtsvg": True,
        "qt/*:qttools": True,
        "qt/*:qttranslations": True,
        "qt/*:qtmultimedia": True,
        "qt/*:qtremoteobjects": True,
        "qt/*:qt5compat": True,
        "qt/*:quick2style": "material"}
    # ---Build---
    generators = []
    # ---Folders---
    no_copy_source = False

    def generate(self):
        VirtualBuildEnv(self).generate()
        CMakeDeps(self).generate()
        tc = CMakeToolchain(self, generator="Ninja")
        qml_import_path = []
        for require, dependency in self.dependencies.items():
            path = dependency.runenv_info.vars(self, scope='run').get("QML_IMPORT_PATH")
            if path is not None:
                qml_import_path.append(path)
        tc.variables["QT_QML_OUTPUT_DIRECTORY"] = "${CMAKE_CURRENT_LIST_DIR}"
        qml_import_path.append("${QT_QML_OUTPUT_DIRECTORY}")
        tc.variables["QML_IMPORT_PATH"] = ";".join(qml_import_path)
        tc.generate()

    def configure(self):
        if self.settings.os == "Linux":
            self.options["qt"].dbus = True

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.settings.os == "Android":
            cmake.build(target="aab")

    def package(self):
        cmake = CMake(self)
        cmake.install()
