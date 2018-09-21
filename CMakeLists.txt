cmake_minimum_required(VERSION 3.10)
project(algobacktester)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(APP_DOMAIN ./domain)
set(APP_CALC ./calc)
set(CMAKE_PREFIX_PATH /Users/evankirkiles/Qt/5.11.1/clang_64)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(algobacktester
        main.cpp
        Infrastructure/data.cpp
        Infrastructure/events.cpp
        Infrastructure/execution.cpp
        Infrastructure/interface.cpp
        Infrastructure/portfolio.cpp
        Graphics/gnuplotter.cpp
        DataHandling/MarketDataFrame.cpp
        DataHandling/YahooFinanceCSVReader.cpp
        Strategies/mainstrategy.cpp
        Strategies/benchmark.cpp
        Graphics/qtwindow.cpp Graphics/montecarlo.cpp Graphics/montecarlo.hpp)

find_package(Boost REQUIRED)
IF (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
endif()
find_package(CURL REQUIRED)
IF (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(algobacktester ${CURL_LIBRARIES})
endif()
find_package(Qt5Widgets REQUIRED)
IF (Qt5Widgets_FOUND)
    include_directories(${Qt5Widgets_INCLUDE_DIRS})
    target_link_libraries(algobacktester ${Qt5Widgets_LIBRARIES})
endif()
find_package(GSL REQUIRED)
IF (GSL_FOUND)
    include_directories(algobacktester ${GSL_LIBRARIES})
    target_link_libraries(algobacktester GSL::gsl GSL::gslcblas)
endif()

include_directories(${APP_DOMAIN})
include_directories(${APP_CALC})