"""
iterative_ensemble_smoother documentation build configuration file.
"""
import datetime
import os
import re
import sys
from subprocess import check_output

import iterative_ensemble_smoother as ies

package_path = os.path.abspath("..")
sys.path.insert(0, package_path)

project = "iterative_ensemble_smoother"
author = "Equinor"
copyright = f"2022-{datetime.datetime.today().year}, {author}"
# The default replacements for |version| and |release|, also used in various
# other places throughout the built documents.
version = re.sub(r"\.dev.*$", r".dev", ies.__version__)
release = version

# convert the python file to a notebook
check_output(["jupytext", "Polynomial.py", "-o", "Polynomial.ipynb"])

# Do the same for this file
check_output(["jupytext", "Oscillator.py", "-o", "Oscillator.ipynb"])

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom ones.
extensions = [
    "sphinx.ext.napoleon",  # autodoc understands numpy docstrings
    "sphinx.ext.autodoc",  # Core library for html generation from docstrings
    "sphinx.ext.autosummary",  # Create neat summary tables
    "sphinx.ext.doctest",  # Test snippets in the documentation
    "sphinx.ext.viewcode",  # Add links to highlighted source code
    "sphinx.ext.intersphinx",  # Link to other projects’ documentation
    "sphinx.ext.autosectionlabel",  # Allow reference sections using its title
    # allows BibTeX citations to be inserted into documentation generated by Sphinx
    "sphinxcontrib.bibtex",
    "jupyter_sphinx",
    "numpydoc",
    "m2r2",  # compatibility with markdown
    "myst_nb",  # to include jupyter notebooks
]

# -----------------------------------------------------------------------------
# Autosummary
# -----------------------------------------------------------------------------

# autosummaries from source-files
autosummary_generate = True
# dont show __init__ docstring
autoclass_content = "class"
# sort class members
autodoc_member_order = "groupwise"
# autodoc_member_order = 'bysource'

autosummary_generate = True  # Turn on sphinx.ext.autosummary
# This is because numpydoc screew up with autosummary
# numpydoc_show_class_members=False

# Napoleon settings
napoleon_google_docstring = True
napoleon_numpy_docstring = True
napoleon_include_init_with_doc = False
napoleon_include_private_with_doc = True
napoleon_include_special_with_doc = False
# napoleon_use_admonition_for_examples = False
# napoleon_use_admonition_for_notes = False
# napoleon_use_admonition_for_references = False
napoleon_use_ivar = True
# napoleon_use_param = True
# napoleon_use_rtype = True

# Add any paths that contain templates here, relative to this directory.
templates_path = ["_templates"]

# Add any external modules you want to refer to in the docs here.
intersphinx_mapping = {
    "python": ("https://docs.python.org/3", None),
    "numpy": ("https://numpy.org/doc/stable/", None),
}

# The suffix(es) of source filenames.
# You can specify multiple suffix as a list of string:
# source_suffix = ['.rst', '.md']
source_suffix = [".rst", ".md", ".ipynb"]

# The encoding of source files.
# source_encoding = 'utf-8-sig'

# The master toctree document.
master_doc = "index"

# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
language = "en"

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = ["build", "_templates/*.rst'", "Thumbs.db", ".DS_Store"]

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = "sphinx"

# -----------------------------------------------------------------------------
# HTML output
# -----------------------------------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
# html_theme = "sphinx_rtd_theme"
html_theme = "pydata_sphinx_theme"

# TODO: when ies gets a nice logo
# html_logo = '_static/logo.svg'
# html_favicon = '_static/favicon.ico'

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
#
html_theme_options = {
    # "google_analytics_id": "UA-140243896-1",
    "navigation_with_keys": False,
    "show_prev_next": False,
    "github_url": "https://github.com/equinor/iterative_ensemble_smoother",
    "icon_links": [
        {
            "name": "Support",
            "url": "https://github.com/equinor/iterative_ensemble_smoother/issues",
            "icon": "fa fa-comment fa-fw",
        },
        # {
        #     "name": "The Paper",
        #     "url": "https://doi.org/10.21105/joss.01450",
        #     "icon": "fa fa-file-text fa-fw",
        # },
    ],
}

# Title displayed on the html page
html_title = "Iterative Ensemble Smoother"

# -----------------------------------------------------------------------------
# Bibliography
# -----------------------------------------------------------------------------

bibtex_bibfiles = ["./refs.bib"]
bibtex_default_style = "unsrt"
bibtex_reference_style = "author_year"
suppress_warnings = ["bibtex.duplicate_citation", "autosectionlabel.*"]

numpydoc_class_members_toctree = False

# -----------------------------------------------------------------------------
# Myst configuration
# -----------------------------------------------------------------------------

# see https://myst-parser.readthedocs.io/en/latest/syntax/optional.html#syntax-mathjax
# for math parsing in jupyter notebooks
myst_enable_extensions = [
    "amsmath",
    "dollarmath",
]
