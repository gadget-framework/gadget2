
getFleetNo <- function(fleetName){
	match(fleetName, getEcosystemInfo()$fleet)
}

getStockNo <- function(stockName){
        match(stockName, getEcosystemInfo()$stock)
}

