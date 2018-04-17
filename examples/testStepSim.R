library(gadgetr)

# Change working directory to example haddock data
loadExample()

# Load parameters
gadget(c("-s","-i","refinputfile"))

# Initialize simulation
initSim()

# Count steps
lengthSteps <- 0

# Loop for all steps (year + step)
# Loop for all years
while (TRUE){
        stop <- stepSim()
        # Stop at the end of time
        if(stop["currentTime"]>=stop["totalSteps"]) break
        lengthSteps <- lengthSteps + 1
}

print(lengthSteps)

# Sim cleanup
finalizeSim()

# Get the output
out <- finalize()

