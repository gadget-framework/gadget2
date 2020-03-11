#!/usr/bin/env Rscript
# Build docs into given directory
library(bookdown)

args <- commandArgs(trailingOnly=TRUE)
project_dir <- getwd()

if (length(args) == 0) {
    stop("Usage: ./build-docs.R (directory-to-build-into | --serve)")
}

if (args[[1]] == '--serve') {
    output_dir <- tempfile(fileext = ".build-docs")
    serve_output <- TRUE
} else {
    output_dir <- args[[1]]
    serve_output <- FALSE
}

# Work out output_dir
dir.create(output_dir, recursive = TRUE, showWarnings = FALSE)
output_dir <- normalizePath(output_dir)  # NB: The path has to exist first for this to work
writeLines(paste("** Building documentation in", output_dir))

writeLines('<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
  </head>
  <body>
    <ul>
      <li><a href="docs/userguide">Gadget userguide</a></li>
      <li><a href="docs/reference">Gadget reference manual</a></li>
    </ul>
  </body>
</html>
', file.path(output_dir, 'index.html'))

userguide_dir <- file.path(output_dir, 'docs/userguide')
dir.create(userguide_dir, recursive = TRUE, showWarnings = FALSE)
tryCatch({
    setwd(file.path(project_dir, "docs/userguide"))
    bookdown::render_book(
        "index.Rmd",
        clean_envir = FALSE,
        output_dir = userguide_dir)
}, finally = setwd(project_dir))

Sys.setenv(DOXYGEN_OUTPUT_DIRECTORY = file.path(output_dir, 'docs/reference'))
if (system("doxygen gadget.dox") != 0) stop("Doxygen failed")

if (serve_output) {
    servr::httd(dir = output_dir, host = "0.0.0.0", port = 8000)
}
