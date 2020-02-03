library(gadget2)
library(unittest, quietly = TRUE)

matching_line <- function(match, lines) {
    any(grepl(match, lines, fixed = TRUE))
}


ok_group("gadget_binary", {
    out <- system(paste(gadget_binary(), '-h'), intern = TRUE)
    ok(matching_line(
        sprintf("Gadget version %s", packageVersion("gadget2")),
        out), "Run gadget binary and get help text with matching version")
    ok(matching_line(
        'http://www.github.com/hafro/gadget',
        out), "Help quoted webpage link (i.e. got to the end of the output")
})
