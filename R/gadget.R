##' Return the location of the gadget binary installed by this package
##' @return File path, e.g. "/usr/local/lib/R/site-library/gadget/bin/gadget"
##' @export
gadget_binary <- function () {
    normalizePath(file.path(
        find.package('gadget2'),
        'bin',
        .Platform$r_arch,  # NB: R_ARCH in src/install.libs.R has a "/", this doesn't
        if (.Platform$OS.type == 'windows') 'gadget.exe' else 'gadget'
        ))
}
