##' Return the location of the gadget binary installed by this package
##' @return File path, e.g. "/usr/local/lib/R/site-library/gadget/bin/gadget"
##' @export
gadget_binary <- function () {
    normalizePath(file.path(
        find.package('gadget'),
        paste0('bin', .Platform$r_arch),
        if (.Platform$OS.type == 'windows') 'gadget.exe' else 'gadget'
        ))
}
