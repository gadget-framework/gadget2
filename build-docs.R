#!/usr/bin/env Rscript
# Build docs into given directory
if(!require(bookdown)) install.packages('bookdown')

args <- commandArgs(trailingOnly=TRUE)
project_dir <- getwd()

if (length(args) == 0) {
    stop("Usage: ./build-docs.R (directory-to-build-into)")
}

# Work out output_dir
output_dir <- args[[1]]
dir.create(output_dir, recursive = TRUE, showWarnings = FALSE)
output_dir <- normalizePath(output_dir)  # NB: The path has to exist first for this to work
writeLines(paste("** Building documentation in", output_dir))

writeLines('<!doctype html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="refresh" content="0;url=docs/userguide/" />
  </head>
  <body>
    <a href="docs/userguide">Gadget documentation</a>
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