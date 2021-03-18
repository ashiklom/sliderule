#
# Connects to SlideRule server at provided url and prints log messages
# generated on server to local terminal 
#

import sys
import logging
from sliderule import sliderule
from sliderule import icesat2

###############################################################################
# GLOBAL CODE
###############################################################################

# configure logging
logging.basicConfig(level=logging.INFO)

###############################################################################
# MAIN
###############################################################################

if __name__ == '__main__':

    # Override server URL from command line
    url = ["127.0.0.1"]
    if len(sys.argv) > 1:
        url = [sys.argv[1]]

    # Override duration to maintain connection
    duration = 0 # seconds
    if len(sys.argv) > 2:
        duration = int(sys.argv[2])

    # Override event type
    event_type = "LOG"
    if len(sys.argv) > 3:
        event_type = sys.argv[3]

    # Override event level
    event_level = "INFO"
    if len(sys.argv) > 4:
        event_level = sys.argv[4]

    # Initialize ICESat2/SlideRule Package
    icesat2.init(url, True)

    # Retrieve logs
    sliderule.tail(event_type, event_level, duration)
