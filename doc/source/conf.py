# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'HoneyGUI'
copyright = '2023, miaomiao_wu'
author = 'miaomiao_wu'
release = 'v0.0.0.1'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ["breathe", "myst_parser", "sphinx_rtd_theme"]

templates_path = ['_templates']
exclude_patterns = []



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_theme_path = ["_themes"]
html_static_path = ['_static']

source_suffix = {
    '.rst': 'restructuredtext',
    '.txt': 'markdown',
    '.md': 'markdown',
}

source_encoding = "utf-8"

master_doc = "index"

breathe_projects = {"HoneyGUI": r"D:\code\Doc\HoneyGUI\doc\doxyxml\xml"}
breathe_default_project = "HoneyGUI"
breathe_domain_by_extension = {"h" : "c"}
breathe_implementation_filename_extensions = ['.c', '.cc', '.cpp']

html_logo = './_static/image/bbpro-h55px.jpg'
html_favicon = "./_static/image/favicon.ico"

html_show_sourcelink = False