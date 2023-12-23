#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json, os
from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain
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
    requires = ["qt/6.6.0@%s/stable" % user,
                "libonvif/3.0.0@%s/snapshot" % user,
                "librtsp/1.0.0@%s/stable" % user,
                "materialrally/[~1]@%s/snapshot" % user,
                "qtappbase/[~1]@%s/snapshot" % user,
                "mdk_sdk/0.22.0@%s/stable" % user,
                "ffmpeg/6.1"
                ]
    tool_requires = ["cmake/3.21.7", "ninja/1.11.1"]
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
        "ffmpeg/*:shared": True,
        "ffmpeg/*:with_asm": False,
        "ffmpeg/*:with_sdl": False,
        "ffmpeg/*:with_ssl": False,
        "ffmpeg/*:with_xcb": False,
        "ffmpeg/*:with_lzma": False,
        "ffmpeg/*:with_opus": False,
        "ffmpeg/*:with_zlib": False,
        "ffmpeg/*:swresample": True,
        "ffmpeg/*:with_bzip2": False,
        "ffmpeg/*:with_pulse": False,
        "ffmpeg/*:with_vaapi": False,
        "ffmpeg/*:with_vdpau": False,
        "ffmpeg/*:with_libvpx": False,
        "ffmpeg/*:with_vorbis": False,
        "ffmpeg/*:with_vulkan": False,
        "ffmpeg/*:with_zeromq": False,
        "ffmpeg/*:with_libalsa": False,
        "ffmpeg/*:with_libwebp": False,
        "ffmpeg/*:with_libx264": False,
        "ffmpeg/*:with_libx265": False,
        "ffmpeg/*:with_freetype": False,
        "ffmpeg/*:with_libiconv": False,
        "ffmpeg/*:with_openh264": False,
        "ffmpeg/*:with_openjpeg": False,
        "ffmpeg/*:with_programs": False,
        "ffmpeg/*:with_libfdk_aac": False,
        "ffmpeg/*:with_libmp3lame": False,
        "qt/*:GUI": True,
        "qt/*:opengl": "desktop",
        "qt/*:qtbase": True,
        "qt/*:widgets": True,
        "qt/*:qtdeclarative": True,
        "qt/*:qtsvg": True,
        "qt/*:qttools": True,
        "qt/*:qttranslations": True,
        "qt/*:qtmultimedia": True,
        "qt/*:qt5compat": True}
    # ---Build---
    generators = []
    # ---Folders---
    no_copy_source = False

    def generate(self):
        ms = VirtualBuildEnv(self)
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
        ms.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        if self.settings.os == "Android":
            cmake.build(target="aab")

    def package(self):
        cmake = CMake(self)
        cmake.install()
