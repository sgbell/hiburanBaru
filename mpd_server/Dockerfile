FROM debian:stretch

RUN adduser mpd

RUN apt -y update

RUN apt -y install git \
                   g++ libpcre3-dev \
                   libmad0-dev libmpg123-dev libid3tag0-dev \
                   libflac-dev libvorbis-dev libopus-dev \
                   libadplug-dev libaudiofile-dev libsndfile1-dev libfaad-dev \
                   libfluidsynth-dev libgme-dev libmikmod2-dev libmodplug-dev \
                   libmpcdec-dev libwavpack-dev \
                   libsidplay2-dev libsidutils-dev libresid-builder-dev \
                   libavcodec-dev libavformat-dev \
                   libmp3lame-dev libtwolame-dev libshine-dev \
                   libsamplerate0-dev libsoxr-dev \
                   libbz2-dev libcdio-paranoia-dev libiso9660-dev libmms-dev \
                   libzzip-dev \
                   libcurl4-gnutls-dev libyajl-dev libexpat-dev \
                   libasound2-dev libao-dev libopenal-dev \
                   libpulse-dev libshout3-dev \
                   libsndio-dev \
                   libmpdclient-dev \
                   libnfs-dev libsmbclient-dev \
                   libsqlite3-dev \
                   libsystemd-dev libgtest-dev libboost-dev \
                   libicu-dev python3 python3-pip python3-setuptools \
		   python3-wheel ninja-build

RUN pip3 install meson

RUN git clone https://github.com/MusicPlayerDaemon/MPD.git

RUN cd MPD && \
    meson . output/release && \
    ninja -C output/release && \
    ninja -C output/release install

RUN mkdir -p /var/lib/mpd && \
    chown mpd.mpd /var/lib/mpd

EXPOSE 6600

ENTRYPOINT [ "mpd", "--stdout", "--no-daemon" , "/etc/mpd.conf" ]
