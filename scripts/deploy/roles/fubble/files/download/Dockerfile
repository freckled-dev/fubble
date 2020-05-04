FROM httpd:2.4
# TODO move `AppImage`s to `download_files`
COPY appimage/*AppImage /usr/local/apache2/htdocs/
COPY download_files/*exe /usr/local/apache2/htdocs/
RUN rm /usr/local/apache2/htdocs/index.html

