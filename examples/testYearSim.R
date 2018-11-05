library(gadgetr)

# Change working directory to example haddock data
loadExample()

# Load parameters
gadget(c("-s","-i","refinputfile"))

# Initialize simulation
initSim()

# Count year
lengthYear <- 0

# Loop for all years
while (TRUE){
	status<- yearSim()
	# Stop at the end of time
        if(status["finished"] == 1) break
	lengthYear <- lengthYear + 1
}

print(lengthYear)

# Sim cleanup
finalizeSim()

# Get the output
out <- finalize()

