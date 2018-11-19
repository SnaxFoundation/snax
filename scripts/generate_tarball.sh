#! /bin/bash

NAME=$1
SNAX_PREFIX=${PREFIX}/${SUBPREFIX}
mkdir -p ${PREFIX}/bin/
#mkdir -p ${PREFIX}/lib/cmake/${PROJECT}
mkdir -p ${SNAX_PREFIX}/bin 
mkdir -p ${SNAX_PREFIX}/licenses/snax
#mkdir -p ${SNAX_PREFIX}/include
#mkdir -p ${SNAX_PREFIX}/lib/cmake/${PROJECT}
#mkdir -p ${SNAX_PREFIX}/cmake
#mkdir -p ${SNAX_PREFIX}/scripts

# install binaries 
cp -R ${BUILD_DIR}/bin/* ${SNAX_PREFIX}/bin 

# install licenses
cp -R ${BUILD_DIR}/licenses/snax/* ${SNAX_PREFIX}/licenses

# install libraries
#cp -R ${BUILD_DIR}/lib/* ${SNAX_PREFIX}/lib

# install cmake modules
#sed "s/_PREFIX_/\/${SPREFIX}/g" ${BUILD_DIR}/modules/SnaxTesterPackage.cmake &> ${SNAX_PREFIX}/lib/cmake/${PROJECT}/SnaxTester.cmake
#sed "s/_PREFIX_/\/${SPREFIX}\/${SSUBPREFIX}/g" ${BUILD_DIR}/modules/${PROJECT}-config.cmake.package &> ${SNAX_PREFIX}/lib/cmake/${PROJECT}/${PROJECT}-config.cmake

# install includes
#cp -R ${BUILD_DIR}/include/* ${SNAX_PREFIX}/include

# make symlinks
#pushd ${PREFIX}/lib/cmake/${PROJECT} &> /dev/null
#ln -sf ../../../${SUBPREFIX}/lib/cmake/${PROJECT}/${PROJECT}-config.cmake ${PROJECT}-config.cmake
#ln -sf ../../../${SUBPREFIX}/lib/cmake/${PROJECT}/SnaxTester.cmake SnaxTester.cmake
#popd &> /dev/null

pushd ${PREFIX}/bin &> /dev/null
for f in `ls ${BUILD_DIR}/bin/`; do
   bn=$(basename $f)
   ln -sf ../${SUBPREFIX}/bin/$bn $bn
done
popd &> /dev/null

tar -cvzf $NAME ./${PREFIX}/*
rm -r ${PREFIX}
