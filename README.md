Gadget is the Globally applicable Area Disaggregated General Ecosystem Toolbox.

Welcome to the Gadget development repository. This will eventually contain all the information you ever wanted to know about Gadget, but were afraid to ask. For the time being, however, the information is a bit sparse but we hope that it will grow quickly.

Gadget is a statistical model of marine ecosystems. Gadget models can be both very data- and computationally- intensive. Various scripts have been written to extract data from data warehouses using [mfdb](github.com/mareframe/mfdb/), a specialised R-package, that can handle the complexities of the disparate datasets that can be used by Gadget when estimating model parameters. To speed up processing time, a parallel processing optimiser (called [Paramin](github.com/Hafro/paramin)) has been developed. In addition to these a specialised R-package, [Rgadget](github.com/bthe/rgadget), has been developed to aid in the development of Gadget models by providing tools to estimate likelihood weights, defining model skeletons, and model diagnostics and prognosis.   

# Prerequisites
To compile Gadget one needs a working C++ compiler and a version of Make installed on the computer:

## Linux
This should be install automatically on most distributions but on ubuntu one needs to install build-essentials:

  sudo apt-get install build-essential
  

## MacOsX
It should be sufficient to install XCode through the Appstore and through XCode's preferences install commandline tools

## Windows
Easiest way is to install [Rtools](https://cran.r-project.org/bin/windows/Rtools/) . During the install process allow the installer to append to the system path.

# Compile and install
Make your way to the source folder and simply type:
  
  make
  
If you want install for all users of the computer move the resulting gadget executable to a location in the system path. On *nix based system you can simply type:
  
  sudo make install 


# Acknowledgements
This project has received funding from an EU grant QLK5-CT199-01609 and the European Union’s Seventh Framework Programme for research, technological development and demonstration under grant agreement no.613571.

