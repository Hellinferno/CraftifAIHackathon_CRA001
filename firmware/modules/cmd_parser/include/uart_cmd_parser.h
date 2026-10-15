#ifndef UART_CMD_PARSER_H
#define UART_CMD_PARSER_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CMD_TYPE_UNKNOWN = 0,
    CMD_TYPE_ALERT,
    CMD_TYPE_CLEAR,
} pipegen_cmd_type_t;

typedef struct {
    pipegen_cmd_type_t type;
    char zone[64];
    char reason[64];
} pipegen_cmd_t;

/**
 * @brief Parses a newline/carriage-return terminated command frame received from PipeGen.
 *
 * Implements FR-006: Closed-Loop UART Perception Command Parser.
 *
 * Valid formats:
 *   - "ALERT:<ZONE>:<REASON>\n" (e.g. "ALERT:Restricted_Zone_A:NO-HARDHAT")
 *   - "CLEAR\n"
 *
 * @param[in] line_buf Incoming raw string buffer.
 * @param[out] cmd_out Structured command parsed from the frame.
 * @return true if successfully parsed into a valid command (ALERT or CLEAR).
 */
bool parse_pipegen_frame(const char *line_buf, pipegen_cmd_t *cmd_out);

#ifdef __cplusplus
}
#endif

#endif // UART_CMD_PARSER_H
