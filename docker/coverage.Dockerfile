### Generate Coverage Report ###
# Define builder stage
FROM team-juzang:base as builder

# Share work directory
COPY . /usr/src/project

WORKDIR /usr/src/project/build
RUN cmake ..
RUN make

WORKDIR /usr/src/project/build_coverage

# Build and test
RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN env CTEST_OUTPUT_ON_FAILURE=1 make coverage
