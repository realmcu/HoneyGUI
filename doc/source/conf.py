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
author = 'RTKIOT GUI'
release = 'v0.0.0.1'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = ["breathe", 
              "myst_parser", 
              "sphinx_rtd_theme",
              "sphinx_multiversion",
              "sphinxcontrib.mermaid",
              "sphinx_copybutton"]

templates_path = ['_templates']
exclude_patterns = []

# The default language to highlight source code in. The default is 'python'.
# The value should be a valid Pygments lexer name, see Showing code examples for more details.


highlight_language = 'c'

# The suffix(es) of source filenames.
# You can specify multiple suffix as a list of string:
#
source_suffix = ['.rst', '.md']

source_encoding = "utf-8"

# The master toctree document.
master_doc = "index"

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

html_theme_options = {
    'collapse_navigation' : False,
    'logo_only': True,
}

html_js_files = [
    'js/custom.js',
    'js/versions.js'
]

# -- Options for LaTeX output ---------------------------------------------

latex_engine = 'xelatex'
latex_use_xindy = False
latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    #
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    #
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    #
    # 'preamble': '',

    # Latex figure (float) alignment
    #
    'figure_align': 'H',

    'inputenc': '',
    'utf8extra': '',
    'classoptions': ',openany,oneside',
    'babel': '\\usepackage{babel}',
    'passoptionstopackages': r'''
\PassOptionsToPackage{bookmarksdepth=5}{hyperref}% depth of pdf bookmarks
''',
    'preamble': r'''
\usepackage{fontspec}
\setmonofont{DejaVu Sans Mono}
\usepackage{silence}
\WarningsOff*
\usepackage[pipeTables=true]{markdown}
''',
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (master_doc, 'RTKIOT GUI.tex', 'RTKIOT GUI Documentation',
     'RTKIOT GUI community', 'manual'),
]


# -- Options for manual page output ---------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    (master_doc, 'RTKIOT GUI', 'RTKIOT GUI Documentation',
     [author], 1)
]


# -- Options for Texinfo output -------------------------------------------

# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (master_doc, 'RTKIOT GUI', 'RTKIOT GUI Documentation',
     author, 'Contributors of RTKIOT GUI', 'One line description of project.',
     'Miscellaneous'),
]

# -- Options for Breathe ----------------------------------------------------

breathe_projects = {"HoneyGUI": os.path.join(os.path.dirname(__file__), r"../doxyxml/xml")}
breathe_default_project = "HoneyGUI"
breathe_domain_by_extension = {"h" : "c"}
breathe_implementation_filename_extensions = ['.c', '.cc', '.cpp']


# -- Options for sphinx-multiversion ----------------------------------------------------

smv_latest_version = 'ramless'


# Example configuration for intersphinx: refer to the Python standard library.

def setup(app):
    app.add_config_value('recommonmark_config', {
            'enable_eval_rst': True,
            'enable_auto_toc_tree': 'True',
            }, True)
    app.add_transform(AutoStructify)
    app.add_css_file('css/custom.css')
    app.add_css_file('css/fontawesome.min.css')
    app.add_css_file('css/auto.number.title.css')