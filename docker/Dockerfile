### Build/test container ###
# Define builder stage
FROM team-juzang:base as builder

# Share work directory
COPY . /usr/src/project
WORKDIR /usr/src/project/build

# Build and test
RUN cmake ..
RUN make
RUN ctest --output-on_failure


### Deploy container ###
# Define deploy stage
FROM ubuntu:focal as deploy

# TODO(samyallal00): Copy server output binary to "."
COPY --from=builder /usr/src/project/build/bin/webserver .

# Added: COPY configuration file from builder stage so it’s accessible in the deploy stage
# Above is noted in spec section "Create a Docker container"
COPY --from=builder /usr/src/project/config/config_file .

# Added: COPY static_folder
RUN mkdir ./static_folder/
COPY --from=builder /usr/src/project/tests/static_folder ./static_folder/

# TODO(samyallal00): Expose some port(s)
EXPOSE 80

# Use ENTRYPOINT to specify the binary name
# TODO(samyallal00): Update with real server name
ENTRYPOINT ["./webserver"]

# Use CMD to specify arguments to ENTRYPOINT
# TODO(samyallal00): Update with real server args
CMD ["config_file"]
