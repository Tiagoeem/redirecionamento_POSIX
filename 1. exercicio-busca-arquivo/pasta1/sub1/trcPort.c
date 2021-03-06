/*******************************************************************************
 * FreeRTOS+Trace v2.3.0 Recorder Library
 * Percepio AB, www.percepio.com
 *
 * trcPort.c
 *
 * Contains all portability issues of the trace recorder library. 
 * See also trcPort.h, where port-specific macros are defined.
 *
 * Terms of Use
 * This software is copyright Percepio AB. The recorder library is free for
 * use together with Percepio products. You may distribute the recorder library
 * in its original form, including modifications in trcPort.c and trcPort.h
 * given that these modification are clearly marked as your own modifications
 * and documented in the initial comment section of these source files. 
 * This software is the intellectual property of Percepio AB and may not be 
 * sold or in other ways commercially redistributed without explicit written 
 * permission by Percepio AB.
 *
 * Disclaimer 
 * The trace tool and recorder library is being delivered to you AS IS and 
 * Percepio AB makes no warranty as to its use or performance. Percepio AB does 
 * not and cannot warrant the performance or results you may obtain by using the 
 * software or documentation. Percepio AB make no warranties, express or 
 * implied, as to noninfringement of third party rights, merchantability, or 
 * fitness for any particular purpose. In no event will Percepio AB, its 
 * technology partners, or distributors be liable to you for any consequential, 
 * incidental or special damages, including any lost profits or lost savings, 
 * even if a representative of Percepio AB has been advised of the possibility 
 * of such damages, or for any claim by any third party. Some jurisdictions do 
 * not allow the exclusion or limitation of incidental, consequential or special 
 * damages, or the exclusion of implied warranties or limitations on how long an 
 * implied warranty may last, so the above limitations may not apply to you.
 *
 * OFFER FROM PERCEPIO:
 * For silicon companies and non-corporate FreeRTOS users (researchers, students
 * , hobbyists or early-phase startups) we have an attractive offer: 
 * Provide a hardware timer port and get a FREE single-user licence for
 * FreeRTOS+Trace Professional Edition. Read more about this offer at 
 * www.percepio.com or contact us directly at support@percepio.com.
 *
 * FreeRTOS+Trace is available as Free Edition and in two premium editions.
 * You may use the premium features during 30 days for evaluation.
 * Download FreeRTOS+Trace at http://www.percepio.com/products/downloads/
 *
 * Copyright Percepio AB, 2012.
 * www.percepio.com
 ******************************************************************************/

#include "trcUser.h"

#if (configUSE_TRACE_FACILITY == 1)

#if (INCLUDE_SAVE_TO_FILE == 1)
static char* prvFileName = NULL;
#endif


/*******************************************************************************
 * uiTraceTickCount
 *
 * This variable is updated by the traceTASK_INCREMENT_TICK macro in the 
 * FreeRTOS tick handler. This does not need to be modified when developing a 
 * new timer port. It is prefered to keep any timer port changes in the HWTC 
 * macro definitions, which typically give sufficient flexibility.
 ******************************************************************************/
uint32_t uiTraceTickCount = 0;

/******************************************************************************
 * uiTracePortGetTimeStamp
 *
 * Returns the current time based on the HWTC macros which provide a hardware
 * isolation layer towards the hardware timer/counter.
 *
 * The HWTC macros and uiTracePortGetTimeStamp is the main porting issue
 * or the trace recorder library. Typically you should not need to change
 * the code of uiTracePortGetTimeStamp if using the HWTC macros.
 *
 * OFFER FROM PERCEPIO:
 * For silicon companies and non-corporate FreeRTOS users (researchers, students
 * , hobbyists or early-phase startups) we have an attractive offer: 
 * Provide a hardware timer port and get a FREE single-user license for
 * FreeRTOS+Trace Professional Edition. Read more about this offer at 
 * www.percepio.com or contact us directly at support@percepio.com.
 ******************************************************************************/
void uiTracePortGetTimeStamp(uint32_t *pTimestamp)
{
    static uint32_t last_traceTickCount = 0;
    static uint32_t last_hwtc_count = 0;
    uint32_t traceTickCount = 0;
    uint32_t hwtc_count = 0;
    
    /* Retrieve HWTC_COUNT only once since the same value should be used all throughout this function. */
#if (HWTC_COUNT_DIRECTION == DIRECTION_INCREMENTING)
    hwtc_count = HWTC_COUNT;
#elif (HWTC_COUNT_DIRECTION == DIRECTION_DECREMENTING)
    hwtc_count = HWTC_PERIOD - HWTC_COUNT;
#else
    Junk text to cause compiler error - HWTC_COUNT_DIRECTION is not set correctly!
    Should be DIRECTION_INCREMENTING or DIRECTION_DECREMENTING
#endif
    
    if (last_traceTickCount - uiTraceTickCount - 1 < 0x80000000)
    {
        /* This means last_traceTickCount is higher than uiTraceTickCount,
        so we have previously compensated for a missed tick.
        Therefore we use the last stored value because that is more accurate. */
        traceTickCount = last_traceTickCount;
    }
    else
    {
        /* Business as usual */
        traceTickCount = uiTraceTickCount;
    }

    /* Check for overflow. May occur if the update of uiTraceTickCount has been 
    delayed due to disabled interrupts. */
    if (traceTickCount == last_traceTickCount && hwtc_count < last_hwtc_count)
    {
        /* A trace tick has occurred but not been executed by the kernel, so we compensate manually. */
        traceTickCount++;
    }
    
    /* Check if the return address is OK, then we perform the calculation. */
    if (pTimestamp)
    {
        /* Get timestamp from trace ticks. Scale down the period to avoid unwanted overflows. */
        *pTimestamp = traceTickCount * (HWTC_PERIOD / HWTC_DIVISOR);
        /* Increase timestamp by (hwtc_count + "lost hardware ticks from scaling down period") / HWTC_DIVISOR. */
        *pTimestamp += (hwtc_count + traceTickCount * (HWTC_PERIOD % HWTC_DIVISOR)) / HWTC_DIVISOR;
    }
    
    /* Store the previous values. */
    last_traceTickCount = traceTickCount;
    last_hwtc_count = hwtc_count;
}

/*******************************************************************************
 * vTracePortEnd
 * 
 * This function is called by the monitor when a recorder stop is detected.
 * This is used by the Win32 port to store the trace to a file. The file path is
 * set using vTracePortSetOutFile.
 ******************************************************************************/
void vTracePortEnd()
{
    vTraceConsoleMessage("\n\r[FreeRTOS+Trace] Running vTracePortEnd.\n\r");

    #if (WIN32_PORT_SAVE_WHEN_STOPPED == 1)
    vTracePortSave();
    #endif

    #if (WIN32_PORT_EXIT_WHEN_STOPPED == 1)
    /* In the FreeRTOS/Win32 demo, this allows for killing the application 
    when the recorder is stopped (e.g., when the buffer is full) */
    system("pause");
    exit(0);
    #endif
}

#if (INCLUDE_SAVE_TO_FILE == 1)
/*******************************************************************************
 * vTracePortSetOutFile
 *
 * Sets the filename/path used in vTracePortSave.
 * This is set in a separate function, since the Win32 port calls vTracePortSave
 * in vTracePortEnd if WIN32_PORT_SAVE_WHEN_STOPPED is set.
 ******************************************************************************/
void vTracePortSetOutFile(char* path)
{
    prvFileName = path;
}

/*******************************************************************************
 * vTracePortSave
 *
 * Saves the trace to a file on a local file system. The path is set in a 
 * separate function, vTracePortSetOutFile, since the Win32 port calls 
 * vTracePortSave in vTracePortEnd if WIN32_PORT_SAVE_WHEN_STOPPED is set.
 ******************************************************************************/
void vTracePortSave()
{
    char buf[180];
    FILE* f;

    if (prvFileName == NULL)
    {
        prvFileName = "FreeRTOSPlusTrace.dump";
        sprintf(buf, "No filename specified, using default \"%s\".", prvFileName);
        vTraceConsoleMessage(buf);
    }

    fopen_s(&f, prvFileName, "wb");
    if (f)
    {
        fwrite(RecorderDataPtr, sizeof(RecorderDataType), 1, f);
        fclose(f);
        sprintf(buf, "\n\r[FreeRTOS+Trace] Saved in: %s\n\r", prvFileName);
        vTraceConsoleMessage(buf);
    }
    else
    {
        sprintf(buf, "\n\r[FreeRTOS+Trace] Failed to write to output file!\n\r");
        vTraceConsoleMessage(buf);
    }
}
#endif
#endif