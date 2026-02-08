# Use Ubuntu 24.04 for native GCC 14 support
FROM ubuntu:24.04

# Prevent interactive prompts during package installation
ARG DEBIAN_FRONTEND=noninteractive

# 1. Install GCC 14, GTest, Python (for pre-commit), and curl
RUN apt-get update && apt-get install -y \
    gcc-14 \
    g++-14 \
    libgtest-dev \
    cmake \
    python3 \
    python3-pip \
    python3-venv \
    curl \
    git \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# 2. Set GCC 14 as the default compiler
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 14 \
    && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 14

# 3. Install Hurl (Orange-OpenSource) via binary .deb package
RUN VERSION=7.1.0 && \
    curl --location --remote-name https://github.com/Orange-OpenSource/hurl/releases/download/$VERSION/hurl_${VERSION}_amd64.deb && \
    apt-get update && apt-get install -y ./hurl_${VERSION}_amd64.deb && \
    rm hurl_${VERSION}_amd64.deb

# 4. Install pre-commit globally via pip
RUN pip3 install --break-system-packages pre-commit

# 5. Adds missing pkg-config
RUN apt-get update && apt-get install -y pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Build and install `zc` tool
RUN git clone --depth=1 https://github.com/koutoftimer/zc.git /tmp/zc
RUN make --directory=/tmp/zc --jobs=$(nproc)
RUN make --directory=/tmp/zc install

# Set workspace for GitHub Actions
WORKDIR /github/workspace

# Default command
CMD ["bash"]
