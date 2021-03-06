10 PRINT "PROGRAM FILE 66: EXCEPTION - SUBSCRIPT TOO LARGE FOR"
15 PRINT "        TWO-DIMENSIONAL ARRAY, WITH DIM."
20 PRINT "    ANSI STANDARD 6.5, 15.2, 15.4"
30 PRINT
40 PRINT "SECTION 66.1: EXCEPTION - SUBSCRIPT TOO LARGE FOR"
50 PRINT "        TWO-DIMENSIONAL ARRAY, WITH DIM."
60 PRINT
70 PRINT "THE OBJECT OF THIS TEST IS TO SEE WHETHER THE PROCESSOR"
80 PRINT "RECOGNIZES A FATAL EXCEPTION WHEN A SUBSCRIPT EXCEEDS"
90 PRINT "THE EXPLICIT UPPER BOUND OF AN ARRAY."
100 PRINT
110 PRINT "TO PASS THIS TEST:"
120 PRINT
130 PRINT "    1) A MESSAGE IDENTIFYING THE EXCEPTION MUST BE"
140 PRINT "       DISPLAYED, AND"
150 PRINT
160 PRINT "    2) THE PROGRAM MUST TERMINATE UPON USE OF THE ILLEGAL"
170 PRINT "       SUBSCRIPT."
180 PRINT
190 PRINT "                               BEGIN TEST."
200 PRINT
210 DIM B(3,12)
220 FOR I=9 TO 14
230   PRINT "ABOUT TO ASSIGN TO B(0,";I;").";
240   IF I<13 THEN 270
250   PRINT "*** EXCEPTION SHOULD OCCUR NOW ***"
260   GOTO 280
270   PRINT " ASSIGNMENT SHOULD BE OK."
280   LET B(0,I) = 20 - I
290   PRINT "HAVE ASSIGNED ";20-I;" TO B(0,";I;")."
300   IF I<13 THEN 320
310   PRINT "ASSIGNMENT TO SUBSCRIPT > 12 - *** TEST FAILS ***"
320   PRINT
330 NEXT I
340 PRINT
350 PRINT "                                END TEST."
360 PRINT
370 PRINT "END PROGRAM 66"
380 END
