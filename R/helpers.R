
getFleetNo <- function(fleetName){
	match(fleetName, getEcosystemInfo()$fleet)
}

getStockNo <- function(stockName){
        match(stockName, getEcosystemInfo()$stock)
}

getLenGrpIdx <- function(stockNo, len){
	stockInfoLen <- getStockInfoC(stockNo)$lengthGroup
	sSize <- nrow(stockInfoLen)

	if(len < stockInfoLen[1, "minLength"])
		return(-1)

	if(len > stockInfoLen[sSize, "maxLength"])
                return(sSize)

	idx <- which(stockInfoLen$minLength <= len & stockInfoLen$maxLength > len)
	return(idx)
}

updateRenewal <- function(stockName, year, step, area, age, number, mean,
                sdev=NA, alpha=NA, beta=NA,
                length=NA, meanWeight=NA){

	# Convert name into number
	stockNo <- getStockNo(stockName)

	if(is.na(stockNo)){
		print("Can't find stock")
		return(1)
	}

	return (updateRenewalC(stockNo, year, step, area, age, number, mean,
                sdev, alpha, beta,
                length, meanWeight))

}

updateSuitability <- function(fleetName, stockName, len, value){

	# Convert stock name into number
        stockNo <- getStockNo(stockName)

        if(is.na(stockNo)){
                print("Can't find stock!")
                return(1)
        }

	# Convert fleet name into number
        fleetNo <- getFleetNo(fleetName)

        if(is.na(fleetNo)){
                print("Can't find fleet!")
                return(1)
        }

	return(updateSuitabilityC(fleetNo, stockNo, len, value))
}

updateAmount <- function(fleetName, year, step, area, value){

	# Convert fleet name into number
        fleetNo <- getFleetNo(fleetName)

        if(is.na(fleetNo)){
                print("Can't find fleet")
                return(1)
        }

        return(updateAmountYear(fleetNo, year, step, area, value))


}

