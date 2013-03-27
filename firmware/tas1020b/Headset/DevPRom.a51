;--------------------------------------------------------------------------
;  Copyright 2000, Texas Instruments Inc.
;--------------------------------------------------------------------------
;  DevPROM.A51:   
;       This used to provide the application FW with a standard entry
;       point for accessing ROM functions.
;--------------------------------------------------------------------------
;
       EXTRN CODE(_DevFunctionEntryParser)
$IF  (_TAS1020_)
       CSEG   AT  002BH
$ELSE
       CSEG   AT  0100H
$ENDIF
       LJMP   _DevFunctionEntryParser   ; Jump to the actual code.
       END
