#Copyright (c) 2009-11, butia

# This procedure is invoked when the user-definable block on the
# "extras" palette is selected.

# Usage: Import this code into a Python (user-definable) block; when
# this code is run, the current hour, minute, and second are pushed to
# the FILO heap. To use these values, pop second, then minute, then
# hour from the FILO.


def myblock(tw, x):  # ignore second argument
    ''' Push seconds onto the FILO. '''

    import time
    tw.lc.heap.append(time.time())

