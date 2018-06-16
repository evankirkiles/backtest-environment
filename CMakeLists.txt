cmake_minimum_required(VERSION 3.10)
project(algobacktester)

set(CMAKE_CXX_STANDARD 11)
set(APP_DOMAIN ./domain)
set(APP_CALC ./calc)

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
        Strategies/strat_buyandhold.cpp
        Strategies/benchmark.cpp
        )

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

include_directories(${APP_DOMAIN})
include_directories(${APP_CALC})