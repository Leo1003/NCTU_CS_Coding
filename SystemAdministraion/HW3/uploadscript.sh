#!/bin/sh
if [ "${UPLOAD_VUSER}" == 'ftp' ]; then
    UPLOAD_VUSER='anonymous'
fi
echo "$(date): [${UPLOAD_VUSER}] has uploaded '$1'; Size: ${UPLOAD_SIZE} bytes" >> /var/log/uploadscript.log
