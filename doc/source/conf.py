# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import recommonmark
from recommonmark.transform import AutoStructify

project = 'RTKIOT GUI'
copyright = '2023, miaomiao_wu'
author = 'miaomiao_wu'
release = 'v0.0.0.1'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ["breathe", 
              "myst_parser", 
              "sphinx_rtd_theme"]

templates_path = ['_templates']
exclude_patterns = []

# The default language to highlight source code in. The default is 'python'.
# The value should be a valid Pygments lexer name, see Showing code examples for more details.


highlight_language = 'c'

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'
html_theme_path = ["_themes"]
html_static_path = ['_static']

html_logo = './_static/image/bbpro-h55px.jpg'
html_favicon = "./_static/image/favicon.ico"

html_show_sourcelink = False

# The default language to highlight source code in. The default is 'python'.
# The value should be a valid Pygments lexer name, see Showing code examples for more details.


highlight_language = 'c'

# The suffix(es) of source filenames.
# You can specify multiple suffix as a list of string:
#
source_suffix = ['.rst', '.md']

source_encoding = "utf-8"

master_doc = "index"

# -- Options for Breathe ----------------------------------------------------

breathe_projects = {"HoneyGUI": os.path.join(os.path.dirname(__file__), r"..\doxyxml\xml")}
breathe_default_project = "HoneyGUI"
breathe_domain_by_extension = {"h" : "c"}
breathe_implementation_filename_extensions = ['.c', '.cc', '.cpp']


# Example configuration for intersphinx: refer to the Python standard library.

def setup(app):
    app.add_config_value('recommonmark_config', {
            'enable_eval_rst': True,
            'enable_auto_toc_tree': 'True',
            }, True)
    app.add_transform(AutoStructify)