There are still some issues with include statements because we are not sure why we should change them in myio.h

All of our read/write functionality works as do our lseek and close functions. Flush is called within close because it was the only way
to make sure the user got what they requested eventually without them explicitly calling myflush.
