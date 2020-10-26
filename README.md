Gadget is the Globally applicable Area Disaggregated General Ecosystem Toolbox.

Welcome to the Gadget development repository. This will eventually contain all the information you ever wanted to know about Gadget, but were afraid to ask. For the time being, however, the information is a bit sparse but we hope that it will grow quickly.

Gadget is a statistical model of marine ecosystems. Gadget models can be both very data- and computationally- intensive. Various scripts have been written to extract data from data warehouses using [mfdb](https://github.com/mareframe/mfdb/), a specialised R-package, that can handle the complexities of the disparate datasets that can be used by Gadget when estimating model parameters. To speed up processing time, a parallel processing optimiser (called [Paramin](https://github.com/Hafro/paramin)) has been developed. In addition to these a specialised R-package, [Rgadget](https://github.com/Hafro/rgadget), has been developed to aid in the development of Gadget models by providing tools to estimate likelihood weights, defining model skeletons, and model diagnostics and prognosis.   

[![Travis build status](https://travis-ci.org/Hafro/gadget2.svg?branch=master)](https://travis-ci.org/Hafro/gadget2)

# Features
Gadget is a software tool that can run complicated statistical ecosystem models, which take many features of the ecosystem into account. Gadget works by running an internal model based on many parameters, and then comparing the data from the output of this model to ''real'' data to get a goodness-of-fit likelihood score. These parameters can then be adjusted, and the model re-run, until an optimum is found, which corresponds to the model with the lowest likelihood score.

Gadget allows you to include a number of features into your model: One or more species, each of which may be split into multiple stocks; multiple areas with migration between areas; predation between and within species; maturation; reproduction and recruitment; multiple commercial and survey fleets taking catches from the populations.

An indication of the features that Gadget can model can be given by
looking at the data input files required shown in the diagram below:

![Features](docs/features.png)


These input data files are in a column-based format, which means that they can easily be generated using simple extraction routines from databases. This means that the construction of a Gadget model can be done reasonably quickly, and the models will be free of any formatting errors in the Gadget input files that could prevent Gadget models from running. The data format is explained in more detail in the [Gadget User Guide](https://hafro.github.io/gadget/docs/userguide). There is also a tutorial available [here](https://heima.hafro.is/~bthe/gadget_course.html)

# Installing as an R package

Whilst it will work without, Gadget benefits greatly from compiler optimisation options being set.
Create a ``~/.R/Makevars`` (Linux/MacOS) or ``~/.R/Makevars.win`` (Windows) file and add the following:

    CFLAGS += -O3 -march=native
    CXXFLAGS += -O3 -march=native

Assuming R and ``remotes`` is installed you can install gadget with:

    remotes::install_github("Hafro/gadget2")

You can find where gadget is installed using the ``gadget_binary`` command:

    > gadget::gadget_binary()
    [1] "/usr/local/lib/R/site-library/gadget/bin/gadget"

If you'd like gadget to be available on the system path, create a symlink as follows:

    sudo ln -s "/usr/local/lib/R/site-library/gadget/bin/gadget" /usr/local/bin/gadget

# Installing as a standalone executable

## Prerequisites
To compile Gadget one needs a working C++ compiler and a version of Make installed on the computer:

### Linux
This should be install automatically on most distributions but on ubuntu one needs to install build-essentials:

>  sudo apt-get install build-essential

If using paramin, you will also need mpic:

> sudo apt install mpich libmpich-dev

### MacOsX
It should be sufficient to install XCode through the Appstore and through XCode's preferences install commandline tools

### Windows
Easiest way is to install [Rtools](https://cran.r-project.org/bin/windows/Rtools/) . During the install process allow the installer to append to the system path.

## Compile and install
To install Gadget you can either [download as a zip file](https://github.com/Hafro/gadget/archive/master.zip) (or clone the repository using git). Unzip this file and within command prompt/terminal application make your way to the resulting folder, typically called `gadget-master`, and simply type:
  
>  make
  
For convenience you may want to install the resulting gadget executable to a location in the system path, thus allowing the gadget executable to called in any directory. On *nix based system you can simply type:
  
>  sudo make install 

# Development

## Testing compilation under FreeBSD Make

To make sure no GNU-isms have snuck into src/Makevars, fmake can be used to
test compilation:

    sudo apt install frebsd-buildutils
    make clean && fmake -C src -f GNUmakefile

## Developing under qtcreator

[qtcreator](https://wiki.qt.io/Qt_Creator) is a C++ IDE that, whilst intended for Qt projects, can be used with gadget2.

Installation under Debian/Ubuntu can be done with:

    apt install gdb qtcreator

Then open the ``gadget2.creator`` project.

Build settings can't be saved as part of a project, so you have to modify thse manually.


* "Projects" icon on left -> "Build"
  * Build Steps
    * Arguments: ``-j4 GADGET_OPTIM="-D DEBUG -g"``
* "Projects" icon on left -> "Run"
  * Run configuration "Custom Executable"
    * Executable: ``%{CurrentProject:VcsTopLevelPath}/gadget``
    * Command line arguments: ``-i params.in``
    * Working directory: ``%{CurrentProject:VcsTopLevelPath}/14-new_ass`` (or wherever your model is stored)
    * Run in terminal: checked

Most code exploration functions, e.g. find definition or uses, can be revealed by right-clicking on any symbol.

Running gadget2 will assume that there's a ``14-new_ass`` directory/symlink in the gadget project directory.
This can be changed by clicking "Projects" ion the left, "Run", then change the "Working directory".

To debug, breakpoints can be added by clicking to the left of a line.
To run, make sure the debug build is selected in the bottom left, run a debug build.
Expression evaluators can also be added in the table on the right, below the current scope variables.

# Acknowledgements
This project has received funding from an EU grant QLK5-CT199-01609 and the European Union’s Seventh Framework Programme for research, technological development and demonstration under grant agreement no.613571.

