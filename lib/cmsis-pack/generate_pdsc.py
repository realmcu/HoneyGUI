#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#

import os
from xml.etree.ElementTree import Element, SubElement, tostring
from xml.dom import minidom

def sanitize_path(path):
    return path.split('/')[-1]

def create_pdsc(vendor, name, version, description, release_date, url, license_name, directory):
    try:
        package = Element("package",
                          schemaVersion="1.7.37",
                          xmlns_xs="http://www.w3.org/2001/XMLSchema-instance",
                          xs_noNamespaceSchemaLocation="https://raw.githubusercontent.com/Open-CMSIS-Pack/Open-CMSIS-Pack-Spec/v1.7.37/schema/PACK.xsd")

        package.set('xmlns:xs', package.attrib.pop('xmlns_xs'))
        package.set('xs:noNamespaceSchemaLocation', package.attrib.pop('xs_noNamespaceSchemaLocation'))

        SubElement(package, "vendor").text = vendor
        sanitized_name = name.replace(".", "_")
        SubElement(package, "name").text = sanitized_name
        SubElement(package, "description").text = description
        SubElement(package, "url").text = url
        SubElement(package, "license").text = license_name

        keywords = SubElement(package, "keywords")
        for keyword in ["CMSIS", "GUI", "HoneyGUI"]:
            SubElement(keywords, "keyword").text = keyword

        releases = SubElement(package, "releases")
        release = SubElement(releases, "release", date=release_date, version=version, tag=f"v{version}", url=url)
        release.text = "- Something About HoneyGUI"

        components = SubElement(package, "components")

        # conditions = SubElement(package, "conditions")
        # # Define conditions corresponding to the configuration options
        # condition_lvgl = SubElement(conditions, "condition", id="CONFIG_REALTEK_BUILD_LVGL_GUI")
        # condition_u8g2 = SubElement(conditions, "condition", id="CONFIG_REALTEK_BUILD_U8G2")

        exclude_files = {"SConscript", "acc_arm_2d.c", "acc_sw_ppe_sim.c"}
        exclude_extensions = {".txt", ".png", ".bmp", ".bin", ".jpg", ".svg", ".js", ".rtkprj", ".rtkui", ".rtkres", ".bak",
                              ".o", ".d", ".a", ".map", ".lst"}
        # Only "resource": the rest of the old list named directories that no
        # longer exist. "font" must not be here -- engine/font/ holds the BMP
        # font renderer that gui_text needs, not just font assets.
        exclude_csub = {"resource"}
        exclude_dirs = {"example", "build", "output", "web", "install", "temp"}

        for root, dirs, files in os.walk(directory):
            if root == directory or any(excluded in root for excluded in exclude_dirs):
                continue

            if files:
                path_parts = root.split(os.sep)
                component_group = path_parts[-1]

                if component_group in exclude_csub:
                    continue

                if any(file not in exclude_files and os.path.splitext(file)[1] not in exclude_extensions for file in files):
                    relative_path = os.path.relpath(root, directory).replace("\\", "/")
                    csub_path = sanitize_path(relative_path)

                    component = SubElement(components, "component",
                                           Cclass="HoneyGUI",
                                           Cversion=version,
                                           Cgroup="realgui",
                                           Csub=csub_path,
                                           Cvariant="Source",
                                           isDefaultVariant="true",
                                           Cvendor=vendor)
                    SubElement(component, "description").text = f"HoneyGUI {relative_path} code, optional."

                    files_element = SubElement(component, "files")

                    for file in files:
                        if file in exclude_files or os.path.splitext(file)[1] in exclude_extensions:
                            continue

                        file_type = "sourceC" if file.endswith((".c", ".cpp")) else "header" if file.endswith(".h") else "other"
                        file_path = os.path.join(relative_path, file).replace("\\", "/")

                        file_sub = SubElement(files_element, "file", category=file_type, name=file_path)

                        # # Apply conditions to specific GUI related files, representing the conditional logic
                        # if file == 'gui_lvgl.c':
                        #     file_sub.set('condition', 'CONFIG_REALTEK_BUILD_LVGL_GUI')

        xml_str = tostring(package, 'utf-8')
        pretty_xml_str = minidom.parseString(xml_str).toprettyxml(indent="  ")

        pdsc_filename = os.path.join(SCRIPT_DIR, f"{vendor}.{sanitized_name}.pdsc")

        with open(pdsc_filename, "w", encoding="utf-8", newline="\n") as pdsc_file:
            pdsc_file.write(pretty_xml_str)

        print(f"{pdsc_filename} generated successfully.")

    except Exception as e:
        print(f"An error occurred: {e}")
        raise

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
REALGUI_DIR = os.path.normpath(os.path.join(SCRIPT_DIR, "..", "..", "realgui"))

create_pdsc(
    vendor="Realtek",
    name="HoneyGUI",
    version="2.0.2",
    description="A simple example CMSIS package for HoneyGUI.",
    release_date="2025-01-21",
    url="https://docs.realmcu.com/gui/en/latest/index.html",
    license_name="LICENSE",
    directory=REALGUI_DIR
)