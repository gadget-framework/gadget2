library(gadgetr)

# Remember to setwd() to the gadget model directory
# Load parameters
gadget(c("-s","-i","refinputfile"))

# Initialize simulation
initSim()

# Placeholder for stop condition
stop <- 0
lengthYear <- 0

# Loop for all steps (year + step)
while (!stop){
	stop <- stepSim()[1]
	lengthYear <- lengthYear + 1
}

print(lengthYear)

# Sim cleanup
finalizeSim()

# Get the output
out <- finalize()

