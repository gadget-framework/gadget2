getPredation <- function(x, type) {
        ecoSystem <- getEcosystemInfo()
        stocks <- ecoSystem[["stock"]]

        getConsumption <- function(prey, fleetNo) {
                stockNo <- getStockNo(prey)
                tryCatch({
                        printPredatorPrey(fleetNo, stockNo, type)
                }, error=function(e){})
        }

	if(type == "fleet") {
                fleetNo <- getFleetNo(x)
	}else if(type == "stock") {
		fleetNo <- getStockNo(x)
	}else{
		print("Type is wrong")
		return(NULL)
	}

        ret <- list()
        catches <- lapply(stocks, getConsumption, fleetNo)
        names(catches) <- stocks
        ret[["catch"]] <- catches
        return(ret)
}

processFleetStats <- function(fleets) {
	stats <- lapply(fleets, getPredation, "fleet")
	names(stats) <- fleets
	return(stats)
}

processStockStats <- function(stocks, pre=TRUE) {
	# For pre (before stepping)
	getStatsPre <- function(x) {
		stockNo <- getStockNo(x)
		ret <- list()
		ret[["stk"]] <- printStock(stockNo)
		return(ret)
	}

	# For post (after stepping)
	getStatsPost <- function(x) {
		stockNo <- getStockNo(x)
		ret <- list()
		ret[["ssb"]] <- printSSB(stockNo)
		ret[["rec"]] <- printRecruitment(stockNo)
		ret[["eat"]] <- getPredation(x, "stock")[["catch"]]
		return(ret)
	}

	if(pre == TRUE)
		stats <- lapply(stocks, getStatsPre)
	else
		stats <- lapply(stocks, getStatsPost)

	names(stats) <- stocks
	return(stats)
}


# Run for a year and collect stats for stocks and fleets for that year
runYear <- function() {

	# Placeholders
	stats <- list()
	tmp <- list()

	# Recursive list combining
	combineStats <- function(x, path) {
		path <- c(path, names(x))
		y <- x[[1]]
		if (class(y) == "list") {
			for(i in names(y))
				combineStats(y[i], path)
		} else {
			# Embed the matrix
			# Must use eval and parse
			eval(parse(text=paste0("xtemp <- tmp", paste0("[[\"", path, "\"]]", collapse=""))))
			eval(parse(text=paste0("ztemp <- stats", paste0("[[\"", path, "\"]]", collapse=""))))

			#print(paste(paste0("xtemp <- tmp", paste0("[[\"", path, "\"]]", collapse="")), "X rows", nrow(xtemp)))
			#print(paste(paste0("ztemp <- stats", paste0("[[\"", path, "\"]]", collapse="")), "Z rows", nrow(ztemp)))

			ztemp <- rbind(ztemp, xtemp)
			#print(paste("comb rows ", nrow(ztemp)))
			eval(parse(text=paste0("stats", paste0("[[\"", path, "\"]]", collapse="")," <<- ztemp")))
			#print(paste("aggregated stats ", nrow(eval(parse(text=paste0("stats", paste0("[[\"", path, "\"]]", collapse="")))))))
		}
	}

	ecoSystem <- getEcosystemInfo()
	currentYear <- ecoSystem[["time"]][["currentYear"]]

	while(TRUE) {
		# Get stats from all stocks (before stepping)
		stockStatPre <- processStockStats(ecoSystem[["stock"]], pre=TRUE)

		# Run forward a single step
		status <- stepSim()

		# Get stats from all stocks (after stepping)
		stockStatPost <- processStockStats(ecoSystem[["stock"]], pre=FALSE)

		# Combine both pre and post
		stockStat <- lapply(ecoSystem[["stock"]], function(x) return(c(stockStatPre[[x]], stockStatPost[[x]])))
		names(stockStat) <- ecoSystem[["stock"]]

		# Get stats from all fleets
		fleetStat <- processFleetStats(ecoSystem[["fleet"]])

		# Make temp list
		tmp <- list(fleets=fleetStat, stocks=stockStat)

		if(length(stats)==0) stats <- tmp
		else {
			combineStats(tmp["fleets"], c())
			combineStats(tmp["stocks"], c())
		}
		# stop condition
		if(status[["currentYear"]] > currentYear || status[["finished"]] == 1) break
	}
	return(stats)
}


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

updateRecruitment <- function(stockName, ...){

        # Convert name into number
        stockNo <- getStockNo(stockName)

        # Process params
        params <- c(...)

        if(is.na(stockNo)){
                print("Can't find stock")
                return(1)
        }

        return (updateRecruitmentC(stockNo, params))
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
