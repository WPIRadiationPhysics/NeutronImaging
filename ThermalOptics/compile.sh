#/bin/bash
## Test run bash macro

# Number of cores
nThreads=2
nEvents=10000

# Test for Geant4 libraries
if [ "$G4COMP" == "" ]; then

    echo "Please define the Geant4 libraries variable to use this macro with \"export G4COMP=/path/to/Geant4/install/lib/Geant4-10.X/\""
    exit
fi

# Rebuild and run application
rm -rf build
mkdir build
cd build
cmake ..
make "-j""$nThreads"
time ./ThermalOptics -t "$nThreads" -n "$nEvents" > run.out
root -q plot.C
