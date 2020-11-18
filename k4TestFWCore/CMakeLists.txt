################################################################################
# Package: k4TestFWCore
################################################################################

find_package(EDM4HEP)
find_package(podio)
find_package(LCIO REQUIRED)

gaudi_subdir(k4TestFWCore v1r0)

gaudi_depends_on_subdirs(GaudiAlg GaudiKernel)


gaudi_add_module(k4TestFWCorePlugins
                 src/components/*.cpp
                 INCLUDE_DIRS k4FWCore EDM4HEP::edm4hep ${LCIO_INCLUDE_DIRS}
                 LINK_LIBRARIES GaudiKernel FWCore  EDM4HEP::edm4hepDict ${LCIO_LIBRARIES})

include(CTest)

gaudi_add_test(CreateExampleEventData
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND gaudirun.py k4TestFWCore/options/createExampleEventData.py)

gaudi_add_test(ReadExampleEventData
               WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
               COMMAND gaudirun.py k4TestFWCore/options/readExampleEventData.py)

