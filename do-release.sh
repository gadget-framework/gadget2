#!/bin/sh
# Script to release package to CRAN.
# Run ./do-release.sh (new-version), and upload the produced tarball to:
#     https://cran.r-project.org/submit.html
set -eu

NEW_VERSION="${1-}"
NEW_SECTION="$(echo "${NEW_VERSION}" | sed -E 's/[^0-9]//g')"
DEV_VERSION="$(echo "${NEW_VERSION}" | sed -E 's/[0-9]+$/99/')"

printf "${NEW_VERSION}" | grep -qE '[0-9]+\.[0-9]+\.[0-9]+' || {
    echo "Usage: $0 (3 part version number, e.g. 2.2.0)"
    exit 1
}

# Set DESCRIPTION / changelog for release
sed -i 's/^Version: .*/Version: '"${NEW_VERSION}"'/' DESCRIPTION
sed -i 's/^Date: .*/Date: '"$(date +%Y-%m-%d)"'/' DESCRIPTION
sed -i 's/Gadget development version {#sec:v9999}/Gadget version '"${NEW_VERSION}"' {#sec:v'"${NEW_SECTION}"'}/' docs/userguide/index.Rmd
git commit -m "Release version ${NEW_VERSION}" DESCRIPTION docs/userguide/index.Rmd
git tag -am "Release version ${NEW_VERSION}" "v${NEW_VERSION}"

# Build tarball of release
R CMD build .

# Set DESCRIPTION / changelog for next development version
sed -i 's/^Version: .*/Version: '"${DEV_VERSION}"'/' DESCRIPTION
cat <<EOF >> docs/userguide/index.Rmd

Gadget development version {#sec:v9999}
---------------------

EOF
git commit -m "Development version ${DEV_VERSION}" DESCRIPTION docs/userguide/index.Rmd
