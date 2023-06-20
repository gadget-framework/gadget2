library(gadget2)
library(unittest, quietly = TRUE)

matching_line <- function(match, lines) {
    if (any(grepl(match, lines, fixed = TRUE))) return(TRUE)
    # Test failed, return the actual output
    return(lines)
}


ok_group("gadget_binary", {
    out <- system2(gadget_binary(), c('-h'), stdout = TRUE, stderr = TRUE)
    ok(matching_line(
        sprintf("Gadget version %s", packageVersion("gadget2")),
        out), "Run gadget binary and get help text with matching version")
    ok(matching_line(
        'https://github.com/gadget-framework/gadget2',
        out), "Help quoted webpage link (i.e. got to the end of the output")
})
